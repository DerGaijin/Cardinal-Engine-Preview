#include "DX11_Swapchain.h"
#include "DX11_DynamicRHI.h"
#include "DX11_RHIImage.h"
#include "CardinalEngine/Application/Window.h"

#include "DX11_Utilities.h"

namespace CE
{
	DX11_Swapchain::DX11_Swapchain(DX11_DynamicRHI* Owner, Window* Window) : m_Owner(Owner)
	{
		DXGI_SWAP_CHAIN_DESC SwapchainDesc{};
		SwapchainDesc.BufferDesc.Width = 0;
		SwapchainDesc.BufferDesc.Height = 0;
		SwapchainDesc.BufferDesc.RefreshRate.Denominator = 0;
		SwapchainDesc.BufferDesc.RefreshRate.Numerator = 0;
		SwapchainDesc.BufferDesc.Format = DX11_Utilities::TranslateDataFormat(m_Owner->Config().SwapchainImageFormat);
		SwapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		SwapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		SwapchainDesc.SampleDesc.Count = 1;
		SwapchainDesc.SampleDesc.Quality = 0;
		SwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapchainDesc.BufferCount = m_Owner->Config().FramesInFlight;
		SwapchainDesc.OutputWindow = Window != nullptr ? (HWND)Window->GetNativeHandle() : NULL;
		SwapchainDesc.Windowed = TRUE;
		SwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapchainDesc.Flags = 0;

		HRESULT Result = m_Owner->GIFactory()->CreateSwapChain(m_Owner->Device().Get(), &SwapchainDesc, &m_Swapchain);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to create Swapchain (%s)", DX11_Utilities::ResultToString(Result));
			return;
		}

		CreateOrUpdateImages();
	}

	bool DX11_Swapchain::IsValid() const
	{
		return m_Swapchain != nullptr;
	}

	DynamicRHI* DX11_Swapchain::Owner() const
	{
		return m_Owner;
	}

	void DX11_Swapchain::Destroy()
	{
		m_Swapchain = nullptr;
	}

	void DX11_Swapchain::Resize(uint32 Width, uint32 Height)
	{
		if (Width == 0 || Height == 0)
		{
			return;
		}

		DXGI_SWAP_CHAIN_DESC SwapchainDesc;
		m_Swapchain->GetDesc(&SwapchainDesc);
		if (Width == SwapchainDesc.BufferDesc.Width && Height == SwapchainDesc.BufferDesc.Height)
		{
			return;
		}

		m_RenderTarget.Reset();

		HRESULT Result = m_Swapchain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, 0);
		CreateOrUpdateImages();
		if (Result != S_OK)
		{
			CE_LOG(ELog::Warning, "Failed to resize Swapchain (%s)", DX11_Utilities::ResultToString(Result));
			return;
		}
	}

	uint32 DX11_Swapchain::ImageCount()
	{
		DXGI_SWAP_CHAIN_DESC SCDesc;
		m_Swapchain->GetDesc(&SCDesc);
		return SCDesc.BufferCount;
	}

	SharedPtr<RHIImage> DX11_Swapchain::Image(uint32 Index) const
	{
		return m_RenderTarget;
	}

	uint32 DX11_Swapchain::CurrentImageIndex() const
	{
		return 0;
	}

	void DX11_Swapchain::CreateOrUpdateImages()
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> BackBuffer;
		m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &BackBuffer);

		DXGI_SWAP_CHAIN_DESC FinalSwapchainDesc{};
		m_Swapchain->GetDesc(&FinalSwapchainDesc);

		m_RenderTarget.Reset();

		RHIImageDescription RenderTargetDesc;
		RenderTargetDesc.Width = FinalSwapchainDesc.BufferDesc.Width;
		RenderTargetDesc.Height = FinalSwapchainDesc.BufferDesc.Height;
		RenderTargetDesc.Format = m_Owner->Config().SwapchainImageFormat;
		RenderTargetDesc.Dimension = ERHIImageDimension::Texture_2D;
		RenderTargetDesc.Usage = ERHIImageUsage::RenderTarget;
		RenderTargetDesc.Access = ERHIAccess::GPUOnly;
		RenderTargetDesc.Samples = ESampleCount::Count_1;
		m_RenderTarget = DynamicRHI::CreateRHIPtr(new DX11_RHIImage(m_Owner, RenderTargetDesc, BackBuffer));
	}
}
