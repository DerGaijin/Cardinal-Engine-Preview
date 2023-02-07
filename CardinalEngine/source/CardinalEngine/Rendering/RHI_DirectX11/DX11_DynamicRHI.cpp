#include "DX11_DynamicRHI.h"
#undef CreateWindow
#include "CardinalEngine/Framework/Platform/Platform.h"

#include "DX11_RHIContextImmediate.h"
#include "DX11_Swapchain.h"
#include "DX11_RHIImage.h"
#include "DX11_RHIBuffer.h"
#include "DX11_Shader.h"
#include "DX11_Pipeline.h"
#include "DX11_Sampler.h"

#include "DX11_Functions.h"
#include "DX11_Utilities.h"

namespace CE
{
	bool DX11_DynamicRHI::Load()
	{
		m_D3D11_LibraryHandle = Platform::Get().LoadDynamicLibrary(L"d3d11");
		if (m_D3D11_LibraryHandle == nullptr)
		{
			CE_LOG(ELog::Warning, "Could not find d3d11.dll");
			return false;
		}

		m_D3DCompiler_LibraryHandle = Platform::Get().LoadDynamicLibrary(L"D3DCompiler_47");
		if (m_D3DCompiler_LibraryHandle == nullptr)
		{
			CE_LOG(ELog::Error, "Could not find D3DCompiler_47.dll");
			return false;
		}

#define EXPORTFUNCTION(LIBRARY, FUNCTION) \
		DX11_Functions::##FUNCTION = (DX11_Functions::##FUNCTION##_t)Platform::Get().GetFunctionPointer(LIBRARY, #FUNCTION); \
		if (DX11_Functions::##FUNCTION == nullptr) \
		{ \
			CE_LOG(ELog::Error, "Failed to export Function: " #FUNCTION); \
			return false; \
		}

		EXPORTFUNCTION(m_D3D11_LibraryHandle, D3D11CreateDevice);
		EXPORTFUNCTION(m_D3DCompiler_LibraryHandle, D3DCompile);
		EXPORTFUNCTION(m_D3DCompiler_LibraryHandle, D3DReflect);
		EXPORTFUNCTION(m_D3DCompiler_LibraryHandle, D3DCreateBlob);

#undef EXPORTFUNCTION
		return true;
	}

	bool DX11_DynamicRHI::Initialize(const RHIConfig& Config)
	{
		DynamicRHI::Initialize(Config);

		IDXGIAdapter* BestAdapter = nullptr;
		D3D_DRIVER_TYPE AdapterDriverType = D3D_DRIVER_TYPE_HARDWARE;
		
		UINT DeviceFlags = 0;
#ifdef CE_DEBUG
		DeviceFlags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif // CE_DEBUG
		
		std::vector<D3D_FEATURE_LEVEL> FeatureLevels = { D3D_FEATURE_LEVEL_11_0 };
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> ImmediateContext = nullptr;
		HRESULT Result = DX11_Functions::D3D11CreateDevice(BestAdapter, AdapterDriverType, NULL, DeviceFlags, FeatureLevels.data(), (UINT)FeatureLevels.size(), D3D11_SDK_VERSION, &m_Device, NULL, &ImmediateContext);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to create Device (%s)", DX11_Utilities::ResultToString(Result));
			return false;
		}

		Microsoft::WRL::ComPtr<IDXGIDevice> GIDevice;
		Microsoft::WRL::ComPtr<IDXGIAdapter> GIAdapter;

		if (m_Device->QueryInterface(IID_PPV_ARGS(&GIDevice)) == S_OK)
		{
			if (GIDevice->GetParent(IID_PPV_ARGS(&GIAdapter)) == S_OK)
			{
				GIAdapter->GetParent(IID_PPV_ARGS(&m_GIFactory));
			}
		}

		if (m_GIFactory == nullptr)
		{
			CE_LOG(ELog::Error, "Failed to query IDXGIFactory");
			return false;
		}

		DXGI_ADAPTER_DESC Desc;
		GIAdapter->GetDesc(&Desc);
		m_Capabilities.DeviceName = Desc.Description;

		m_Immediate = new DX11_RHIContextImmediate(this, ImmediateContext);

		return true;
	}

	void DX11_DynamicRHI::Tick(float DeltaTime)
	{

	}

	void DX11_DynamicRHI::Terminate()
	{
		if (m_Immediate != nullptr)
		{
			RHIObjectDeleter<DX11_RHIContextImmediate>()(m_Immediate);
			m_Immediate = nullptr;
		}

		m_GIFactory = nullptr;
		m_Device = nullptr;

		DynamicRHI::Terminate();
	}

	void DX11_DynamicRHI::Unload()
	{
		DX11_Functions::D3D11CreateDevice = nullptr;
		DX11_Functions::D3DCompile = nullptr;
		DX11_Functions::D3DReflect = nullptr;
		DX11_Functions::D3DCreateBlob = nullptr;

		Platform::Get().UnloadDynamicLibrary(m_D3D11_LibraryHandle);
		Platform::Get().UnloadDynamicLibrary(m_D3DCompiler_LibraryHandle);
	}

	SharedPtr<Swapchain> DX11_DynamicRHI::CreateSwapchain(Window* Window)
	{
		SharedPtr<Swapchain> Result = CreateRHIPtr(new DX11_Swapchain(this, Window));
		if (Result != nullptr)
		{
			SetWindowSwapchain(Window, Result);
		}
		return Result;
	}

	SharedPtr<RHIImage> DX11_DynamicRHI::CreateImage(const RHIImageDescription& Description)
	{
		return CreateRHIPtr(new DX11_RHIImage(this, Description));
	}

	SharedPtr<RHIBuffer> DX11_DynamicRHI::CreateBuffer(const RHIBufferDescription& Description)
	{
		return CreateRHIPtr(new DX11_RHIBuffer(this, Description));
	}

	SharedPtr<VertexShader> DX11_DynamicRHI::CreateVertexShader(const char* Data, size_t DataSize, const char* Entry)
	{
		return CreateRHIPtr(new DX11_VertexShader(this, Data, DataSize, Entry));
	}

	SharedPtr<TessellationControlShader> DX11_DynamicRHI::CreateTessellationControlShader(const char* Data, size_t DataSize, const char* Entry)
	{
		return CreateRHIPtr(new DX11_TessellationControlShader(this, Data, DataSize, Entry));
	}

	SharedPtr<TessellationEvaluationShader> DX11_DynamicRHI::CreateTessellationEvaluationShader(const char* Data, size_t DataSize, const char* Entry)
	{
		return CreateRHIPtr(new DX11_TessellationEvaluationShader(this, Data, DataSize, Entry));
	}

	SharedPtr<GeometryShader> DX11_DynamicRHI::CreateGeometryShader(const char* Data, size_t DataSize, const char* Entry)
	{
		return CreateRHIPtr(new DX11_GeometryShader(this, Data, DataSize, Entry));
	}

	SharedPtr<FragmentShader> DX11_DynamicRHI::CreateFragmentShader(const char* Data, size_t DataSize, const char* Entry)
	{
		return CreateRHIPtr(new DX11_FragmentShader(this, Data, DataSize, Entry));
	}

	SharedPtr<ComputeShader> DX11_DynamicRHI::CreateComputeShader(const char* Data, size_t DataSize, const char* Entry)
	{
		return CreateRHIPtr(new DX11_ComputeShader(this, Data, DataSize, Entry));
	}

	SharedPtr<RHIPipeline_Graphic> DX11_DynamicRHI::CreatePipelineGraphic(const RHIPipeline_GraphicShaders& Shaders, const RHIPipeline_GraphicDescription& Description)
	{
		return CreateRHIPtr(new DX11_RHIPipeline_Grpahic(this, Shaders, Description));
	}

	SharedPtr<Sampler> DX11_DynamicRHI::CreateSampler(const SamplerDescription& Description)
	{
		return CreateRHIPtr(new DX11_Sampler(this, Description));
	}

	RHIContextImmediate& DX11_DynamicRHI::Immediate() const
	{
		return *m_Immediate;
	}
}
