#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"

#include "DirectX11/D3D11.h"
#include <wrl.h>

namespace CE
{
	class DX11_RHIContextImmediate;

	class DX11_DynamicRHI : public DynamicRHI
	{
	public:
		virtual bool Load() override;

		virtual bool Initialize(const RHIConfig& Config) override;

		virtual ERHIType Type() const override
		{
			return ERHIType::DirectX11;
		}

		virtual void Tick(float DeltaTime) override;

		virtual void Terminate() override;

		virtual void Unload() override;

	public:
		virtual SharedPtr<Swapchain> CreateSwapchain(Window* Window) override;

		virtual SharedPtr<RHIImage> CreateImage(const RHIImageDescription& Description) override;

		virtual SharedPtr<RHIBuffer> CreateBuffer(const RHIBufferDescription& Description) override;

		virtual SharedPtr<VertexShader> CreateVertexShader(const char* Data, size_t DataSize, const char* Entry) override;
		virtual SharedPtr<TessellationControlShader> CreateTessellationControlShader(const char* Data, size_t DataSize, const char* Entry) override;
		virtual SharedPtr<TessellationEvaluationShader> CreateTessellationEvaluationShader(const char* Data, size_t DataSize, const char* Entry) override;
		virtual SharedPtr<GeometryShader> CreateGeometryShader(const char* Data, size_t DataSize, const char* Entry) override;
		virtual SharedPtr<FragmentShader> CreateFragmentShader(const char* Data, size_t DataSize, const char* Entry) override;
		virtual SharedPtr<ComputeShader> CreateComputeShader(const char* Data, size_t DataSize, const char* Entry) override;

		virtual SharedPtr<RHIPipeline_Graphic> CreatePipelineGraphic(const RHIPipeline_GraphicShaders& Shaders, const RHIPipeline_GraphicDescription& Description) override;

		virtual SharedPtr<Sampler> CreateSampler(const SamplerDescription& Description) override;

	public:
		const Microsoft::WRL::ComPtr<ID3D11Device>& Device() const
		{
			return m_Device;
		}

		const Microsoft::WRL::ComPtr<IDXGIFactory>& GIFactory() const
		{
			return m_GIFactory;
		}

	protected:
		RHIContextImmediate& Immediate() const override;

	private:
		void* m_D3D11_LibraryHandle = nullptr;
		void* m_D3DCompiler_LibraryHandle = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Device> m_Device = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory> m_GIFactory = nullptr;

		DX11_RHIContextImmediate* m_Immediate = nullptr;

	};
}
