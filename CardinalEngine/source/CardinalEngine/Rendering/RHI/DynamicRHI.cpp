#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Framework/Platform/Platform.h"
#include "CardinalEngine/Application/Window.h"
#include "CardinalEngine/Rendering/ShaderCompiler.h"

#if CE_PLATFORM == CE_PLATFORM_WINDOWS
#include "../RHI_DirectX11/DX11_DynamicRHI.h"
// Include DirectX12
#endif
#if CE_PLATFORM != CE_PLATFORM_APPLE
// Include Vulkan
#endif

namespace CE
{
	uint32 DynamicRHI::SizeOfDataFormat(EDataFormat Format)
	{
		switch (Format)
		{
		case CE::EDataFormat::RGBA32_Typeless:
		case CE::EDataFormat::RGBA32_Float:
		case CE::EDataFormat::RGBA32_UInt:
		case CE::EDataFormat::RGBA32_Int:
			return 4 * 4;
		case CE::EDataFormat::RGB32_Typeless:
		case CE::EDataFormat::RGB32_Float:
		case CE::EDataFormat::RGB32_UInt:
		case CE::EDataFormat::RGB32_Int:
			return 3 * 4;
		case CE::EDataFormat::RGBA16_Typeless:
		case CE::EDataFormat::RGBA16_Float:
		case CE::EDataFormat::RGBA16_UNorm:
		case CE::EDataFormat::RGBA16_UInt:
		case CE::EDataFormat::RGBA16_SNorm:
		case CE::EDataFormat::RGBA16_Int:
			return 4 * 2;
		case CE::EDataFormat::RG32_Typeless:
		case CE::EDataFormat::RG32_Float:
		case CE::EDataFormat::RG32_UInt:
		case CE::EDataFormat::RG32_Int:
			return 2 * 4;
		case CE::EDataFormat::RGBA8_Typeless:
		case CE::EDataFormat::RGBA8_UNorm:
		case CE::EDataFormat::RGBA8_UNorm_SRGB:
		case CE::EDataFormat::RGBA8_UInt:
		case CE::EDataFormat::RGBA8_SNorm:
		case CE::EDataFormat::RGBA8_Int:
		case CE::EDataFormat::BGRA8_UNorm:
		case CE::EDataFormat::BGRA8_UNorm_SRGB:
		case CE::EDataFormat::BGRA8_SNorm:
		case CE::EDataFormat::BGRA8_UInt:
		case CE::EDataFormat::BGRA8_Int:
			return 4 * 1;
		case CE::EDataFormat::RG16_Typeless:
		case CE::EDataFormat::RG16_Float:
		case CE::EDataFormat::RG16_UNorm:
		case CE::EDataFormat::RG16_UInt:
		case CE::EDataFormat::RG16_SNorm:
		case CE::EDataFormat::RG16_Int:
			return 2 * 2;
		case CE::EDataFormat::R32_Typeless:
		case CE::EDataFormat::D32_Float:
		case CE::EDataFormat::R32_Float:
		case CE::EDataFormat::R32_UInt:
		case CE::EDataFormat::R32_Int:
			return 1 * 4;
		case CE::EDataFormat::RG8_Typeless:
		case CE::EDataFormat::RG8_UNorm:
		case CE::EDataFormat::RG8_UInt:
		case CE::EDataFormat::RG8_SNorm:
		case CE::EDataFormat::RG8_Int:
			return 2 * 1;
		case CE::EDataFormat::R16_Typeless:
		case CE::EDataFormat::R16_Float:
		case CE::EDataFormat::D16_UNorm:
		case CE::EDataFormat::R16_UNorm:
		case CE::EDataFormat::R16_UInt:
		case CE::EDataFormat::R16_SNorm:
		case CE::EDataFormat::R16_Int:
			return 1 * 2;
		case CE::EDataFormat::R8_Typeless:
		case CE::EDataFormat::R8_UNorm:
		case CE::EDataFormat::R8_UInt:
		case CE::EDataFormat::R8_SNorm:
		case CE::EDataFormat::R8_Int:
		case CE::EDataFormat::A8_UNorm:
			return 1 * 1;
		}

		return 0;
	}

	bool DynamicRHI::IsRHIAvailable(ERHIType Type)
	{
		switch (Type)
		{
#if CE_PLATFORM != CE_PLATFORM_APPLE
		case CE::ERHIType::Vulkan:
			return false;
			return IsLibraryAvailable(L"vulkan-1");
#endif
#if CE_PLATFORM == CE_PLATFORM_WINDOWS
		case CE::ERHIType::DirectX12:
			return false;
		case CE::ERHIType::DirectX11:
			return IsLibraryAvailable(L"d3d11") && IsLibraryAvailable(L"D3DCompiler_47");
#endif
		}

		return false;
	}

	ERHIType DynamicRHI::ChooseRHIType()
	{
		ERHIType Result = ERHIType::None;

		bool IsVulkanAvailable = IsRHIAvailable(ERHIType::Vulkan);
		if (Result == ERHIType::None && IsVulkanAvailable && StartArgument::HasFlag(L"vulkan"))
		{
			Result = ERHIType::Vulkan;
		}

		bool IsDirectX12Available = IsRHIAvailable(ERHIType::DirectX12);
		if (Result == ERHIType::None && IsDirectX12Available && StartArgument::HasAnyFlag({ L"directx12", L"dx12" }))
		{
			Result = ERHIType::DirectX12;
		}

		bool IsDirectX11Available = IsRHIAvailable(ERHIType::DirectX11);
		if (Result == ERHIType::None && IsDirectX11Available && StartArgument::HasAnyFlag({ L"directx11", L"dx11" }))
		{
			Result = ERHIType::DirectX11;
		}


		if (Result == ERHIType::None && IsVulkanAvailable)
		{
			Result = ERHIType::Vulkan;
		}

		if (Result == ERHIType::None && IsDirectX12Available)
		{
			Result = ERHIType::DirectX12;
		}

		if (Result == ERHIType::None && IsDirectX11Available)
		{
			Result = ERHIType::DirectX11;
		}

		return Result;
	}

	DynamicRHI* DynamicRHI::Create(ERHIType Type)
	{
		switch (Type)
		{
#if CE_PLATFORM != CE_PLATFORM_APPLE
		case CE::ERHIType::Vulkan:
			return nullptr;
#endif
#if CE_PLATFORM == CE_PLATFORM_WINDOWS
		case CE::ERHIType::DirectX12:
			return false;
		case CE::ERHIType::DirectX11:
			return new DX11_DynamicRHI();
#endif
		}

		return nullptr;
	}

	bool DynamicRHI::Initialize(const RHIConfig& Config)
	{
		m_Config = Config;

		ShaderCompiler::Initialize();

		return true;
	}

	void DynamicRHI::Terminate()
	{
		ShaderCompiler::Terminate();
	}

	void DynamicRHI::SetWindowSwapchain(Window* Window, const SharedPtr<Swapchain>& Swapchain)
	{
		if (Window != nullptr)
		{
			Window->m_Swapchain = Swapchain;
		}
	}

	bool DynamicRHI::IsLibraryAvailable(const wchar_t* Lib)
	{
		//@TODO: This is very inefficient, try to find another method
		void* Handle = Platform::Get().LoadDynamicLibrary(Lib);
		bool ValidHandle = Handle != nullptr;
		Platform::Get().UnloadDynamicLibrary(Handle);
		return ValidHandle;
	}
}
