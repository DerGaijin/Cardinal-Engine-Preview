#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Rendering/RHI/Swapchain.h"

#include "DirectX11/D3D11.h"
#include <wrl.h>

namespace CE
{
	class Window;
	class DX11_DynamicRHI;
	class DX11_RHIImage;

	class DX11_Swapchain : public Swapchain
	{
	public:
		DX11_Swapchain(DX11_DynamicRHI* Owner, Window* Window);

		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

	protected:
		virtual void Destroy() override;

	public:
		virtual void Resize(uint32 Width, uint32 Height) override;

		virtual uint32 ImageCount() override;

		virtual SharedPtr<RHIImage> Image(uint32 Index) const override;

		virtual uint32 CurrentImageIndex() const override;


	public:
		const Microsoft::WRL::ComPtr<IDXGISwapChain>& NativeSwapchain() const
		{
			return m_Swapchain;
		}

	private:
		void CreateOrUpdateImages();

	private:
		DX11_DynamicRHI* m_Owner = nullptr;

		Microsoft::WRL::ComPtr<IDXGISwapChain> m_Swapchain = nullptr;

		SharedPtr<DX11_RHIImage> m_RenderTarget;
	};
}
