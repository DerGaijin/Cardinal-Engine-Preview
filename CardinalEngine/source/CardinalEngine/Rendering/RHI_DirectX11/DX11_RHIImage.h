#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Rendering/RHI/RHIImage.h"

#include "DirectX11/D3D11.h"
#include <wrl.h>

namespace CE
{
	class DX11_DynamicRHI;

	class DX11_RHIImage : public RHIImage
	{
	public:
		DX11_RHIImage(DX11_DynamicRHI* Owner, const RHIImageDescription& Desc);

		DX11_RHIImage(DX11_DynamicRHI* Owner, const RHIImageDescription& Desc, const Microsoft::WRL::ComPtr<ID3D11Resource>& NativeImage);
		
		~DX11_RHIImage();

		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

	protected:
		virtual void Destroy() override;

	private:
		void CreateImageViews();

	public:
		const Microsoft::WRL::ComPtr<ID3D11Resource>& NativeImage() const
		{
			return m_Image;
		}

		Microsoft::WRL::ComPtr<ID3D11Texture1D> Texture1D() const
		{
			Microsoft::WRL::ComPtr<ID3D11Texture1D> Temp;
			m_Image.As(&Temp);
			return Temp;
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D() const
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> Temp;
			m_Image.As(&Temp);
			return Temp;
		}

		Microsoft::WRL::ComPtr<ID3D11Texture3D> Texture3D() const
		{
			Microsoft::WRL::ComPtr<ID3D11Texture3D> Temp;
			m_Image.As(&Temp);
			return Temp;
		}

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() const
		{
			return m_ShaderResourceView;
		}

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const
		{
			return m_RenderTargetView;
		}

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDepthStencilView()
		{
			return m_DepthStencilView;
		}

	private:
		DX11_DynamicRHI* m_Owner = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Resource> m_Image = nullptr;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView = nullptr;
	};
}
