#include "DX11_RHIImage.h"
#include "DX11_DynamicRHI.h"

#include "DX11_Utilities.h"

namespace CE
{
	DX11_RHIImage::DX11_RHIImage(DX11_DynamicRHI* Owner, const RHIImageDescription& Desc) : m_Owner(Owner), RHIImage(Desc)
	{
		D3D11_SUBRESOURCE_DATA SubData{};
		SubData.pSysMem = nullptr;
		SubData.SysMemPitch = Description.Width * (uint32)DynamicRHI::SizeOfDataFormat(Description.Format);

		D3D11_USAGE Usage = D3D11_USAGE_DEFAULT;
		uint32 CPUAccessFlags = 0;
		switch (Description.Access)
		{
		case CE::ERHIAccess::GPUOnly:
			Usage = D3D11_USAGE_DEFAULT;
			CPUAccessFlags = 0;
			break;
		case CE::ERHIAccess::GPURead_CPUWrite:
			Usage = D3D11_USAGE_DYNAMIC;
			CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		case CE::ERHIAccess::All:
			Usage = D3D11_USAGE_STAGING;
			CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			break;
		}


		uint32_t BindFlags = 0;
		if (Description.Access != ERHIAccess::All)
		{
			if (Description.Usage & ERHIImageUsage::ShaderResource)
				BindFlags |= D3D11_BIND_SHADER_RESOURCE;
			if (Description.Usage & ERHIImageUsage::RenderTarget)
				BindFlags |= D3D11_BIND_RENDER_TARGET;
			if (Description.Usage & ERHIImageUsage::DepthStencil)
				BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		}

		switch (Description.Dimension)
		{
		default:
		case CE::ERHIImageDimension::Undefined:
			CE_LOG(ELog::Error, "Failed to create Image (Image Dimension undefined)");
			return;
		case CE::ERHIImageDimension::Texture_1D:
		{
			D3D11_TEXTURE1D_DESC Desc{};
			Desc.ArraySize = Description.Depth;
			Desc.BindFlags = BindFlags;
			Desc.CPUAccessFlags = CPUAccessFlags;
			Desc.Format = DX11_Utilities::TranslateDataFormat(Description.Format);
			Desc.MipLevels = Description.MipLevels;
			Desc.MiscFlags = 0;
			Desc.Usage = Usage;
			Desc.Width = Description.Width;

			Microsoft::WRL::ComPtr<ID3D11Texture1D> Texture1D = nullptr;
			HRESULT Result = m_Owner->Device()->CreateTexture1D(&Desc, nullptr, &Texture1D);
			if (Result != S_OK)
			{
				CE_LOG(ELog::Error, "Failed to create Image 1D (%s)", DX11_Utilities::ResultToString(Result));
				Destroy();
				return;
			}
			m_Image = Texture1D;
			break;
		}
		case CE::ERHIImageDimension::Texture_2D:
		{
			D3D11_TEXTURE2D_DESC Desc{};
			Desc.ArraySize = Description.Depth;
			Desc.BindFlags = BindFlags;
			Desc.CPUAccessFlags = CPUAccessFlags;
			Desc.Format = DX11_Utilities::TranslateDataFormat(Description.Format);
			Desc.Height = Description.Height;
			Desc.MipLevels = Description.MipLevels;
			Desc.MiscFlags = 0;
			Desc.SampleDesc.Count = (uint32_t)Description.Samples;
			Desc.SampleDesc.Quality = 0;
			Desc.Width = Description.Width;
			Desc.Usage = Usage;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D = nullptr;
			HRESULT Result = m_Owner->Device()->CreateTexture2D(&Desc, nullptr, &Texture2D);
			if (Result != S_OK)
			{
				CE_LOG(ELog::Error, "Failed to create Image 2D (%s)", DX11_Utilities::ResultToString(Result));
				Destroy();
				return;
			}
			m_Image = Texture2D;
			break;
		}
		case CE::ERHIImageDimension::Texture_3D:
		{
			D3D11_TEXTURE3D_DESC Desc{};
			Desc.BindFlags = BindFlags;
			Desc.CPUAccessFlags = CPUAccessFlags;
			Desc.Depth = 1;
			Desc.Format = DX11_Utilities::TranslateDataFormat(Description.Format);
			Desc.Height = Description.Height;
			Desc.MipLevels = Description.MipLevels;
			Desc.MiscFlags = 0;
			Desc.Width = Description.Width;
			Desc.Usage = Usage;

			Microsoft::WRL::ComPtr<ID3D11Texture3D> Texture3D = nullptr;
			HRESULT Result = m_Owner->Device()->CreateTexture3D(&Desc, nullptr, &Texture3D);
			if (Result != S_OK)
			{
				CE_LOG(ELog::Error, "Failed to create Image 3D (%s)", DX11_Utilities::ResultToString(Result));
				Destroy();
				return;
			}
			m_Image = Texture3D;
			break;
		}
		}

		if (Description.Access != ERHIAccess::All)
		{
			CreateImageViews();
		}
	}

	DX11_RHIImage::DX11_RHIImage(DX11_DynamicRHI* Owner, const RHIImageDescription& Desc, const Microsoft::WRL::ComPtr<ID3D11Resource>& NativeImage)
		: m_Owner(Owner), RHIImage(Desc), m_Image(NativeImage)
	{
		CreateImageViews();
	}

	DX11_RHIImage::~DX11_RHIImage()
	{
	}

	bool DX11_RHIImage::IsValid() const
	{
		return m_Image != nullptr;
	}

	DynamicRHI* DX11_RHIImage::Owner() const
	{
		return m_Owner;
	}

	void DX11_RHIImage::Destroy()
	{
		m_Image = nullptr;
		m_ShaderResourceView = nullptr;
		m_RenderTargetView = nullptr;
		m_DepthStencilView = nullptr;
	}

	void DX11_RHIImage::CreateImageViews()
	{
		DXGI_FORMAT Format = DX11_Utilities::TranslateDataFormat(Description.Format);

		if (Description.Usage & ERHIImageUsage::DepthStencil)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC ViewDesc{};
			ViewDesc.Format = Format;
			ViewDesc.Flags = 0;
			switch (Description.Dimension)
			{
			case CE::ERHIImageDimension::Texture_1D:
				if (Description.Depth == 1)
				{
					ViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
					ViewDesc.Texture1D.MipSlice = 0;
				}
				else
				{
					ViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
					ViewDesc.Texture1DArray.ArraySize = Description.Depth;
					ViewDesc.Texture1DArray.FirstArraySlice = 0;
					ViewDesc.Texture1DArray.MipSlice = 0;
				}
				break;
			case CE::ERHIImageDimension::Texture_2D:
				if (Description.Depth == 1)
				{
					ViewDesc.ViewDimension = Description.Samples == ESampleCount::Count_1 ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
					ViewDesc.Texture2D.MipSlice = 0;
				}
				else
				{
					ViewDesc.ViewDimension = Description.Samples == ESampleCount::Count_1 ? D3D11_DSV_DIMENSION_TEXTURE2DARRAY : D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
					ViewDesc.Texture2DArray.ArraySize = Description.Depth;
					ViewDesc.Texture2DArray.FirstArraySlice = 0;
					ViewDesc.Texture2DArray.MipSlice = 0;
				}
				break;
			case CE::ERHIImageDimension::Texture_3D:
				CE_LOG(ELog::Error, "Texture 3D is not supported for a DepthStencil View");
				return;
			case CE::ERHIImageDimension::Texture_Cube:
				CE_LOG(ELog::Error, "Texture Cube is not supported for a DepthStencil View");
				return;
			default:
				break;
			}

			HRESULT Result = m_Owner->Device()->CreateDepthStencilView(m_Image.Get(), &ViewDesc, &m_DepthStencilView);
			if (Result != S_OK)
			{
				CE_LOG(ELog::Error, "Failed to create Depth Stencil View (%s)", DX11_Utilities::ResultToString(Result));
				Destroy();
				return;
			}
		}

		if (Description.Usage & ERHIImageUsage::RenderTarget)
		{
			D3D11_RENDER_TARGET_VIEW_DESC ViewDesc{};
			ViewDesc.Format = Format;
			switch (Description.Dimension)
			{
			case CE::ERHIImageDimension::Texture_1D:
				if (Description.Depth == 1)
				{
					ViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
					ViewDesc.Texture1D.MipSlice = 0;
				}
				else
				{
					ViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
					ViewDesc.Texture1DArray.ArraySize = Description.Depth;
					ViewDesc.Texture1DArray.FirstArraySlice = 0;
					ViewDesc.Texture1DArray.MipSlice = 0;
				}
				break;
			case CE::ERHIImageDimension::Texture_2D:
				if (Description.Depth == 1)
				{
					ViewDesc.ViewDimension = Description.Samples == ESampleCount::Count_1 ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DMS;
					ViewDesc.Texture2D.MipSlice = 0;
				}
				else
				{
					ViewDesc.ViewDimension = Description.Samples == ESampleCount::Count_1 ? D3D11_RTV_DIMENSION_TEXTURE2DARRAY : D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
					ViewDesc.Texture2DArray.ArraySize = Description.Depth;
					ViewDesc.Texture2DArray.FirstArraySlice = 0;
					ViewDesc.Texture2DArray.MipSlice = 0;
				}
				break;
			case CE::ERHIImageDimension::Texture_3D:
				ViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
				ViewDesc.Texture3D.FirstWSlice = 0;
				ViewDesc.Texture3D.MipSlice = 0;
				ViewDesc.Texture3D.WSize = 1;
				break;
			case CE::ERHIImageDimension::Texture_Cube:
				CE_LOG(ELog::Error, "Texture Cube is not supported for a RenderTarget View");
				return;
			default:
				break;
			}

			HRESULT Result = m_Owner->Device()->CreateRenderTargetView(m_Image.Get(), &ViewDesc, &m_RenderTargetView);
			if (Result != S_OK)
			{
				CE_LOG(ELog::Error, "Failed to create Render Target View (%s)", DX11_Utilities::ResultToString(Result));
				Destroy();
				return;
			}
		}

		if (Description.Usage & ERHIImageUsage::ShaderResource)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC ViewDesc{};
			ViewDesc.Format = Format;
			switch (Description.Dimension)
			{
			case CE::ERHIImageDimension::Texture_1D:
				if (Description.Depth == 1)
				{
					ViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
					ViewDesc.Texture1D.MipLevels = Description.MipLevels;
					ViewDesc.Texture1D.MostDetailedMip = Description.MipLevels > 0 ? Description.MipLevels - 1 : 0;
				}
				else
				{
					ViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
					ViewDesc.Texture1DArray.MipLevels = Description.MipLevels;
					ViewDesc.Texture1DArray.MostDetailedMip = Description.MipLevels > 0 ? Description.MipLevels - 1 : 0;
					ViewDesc.Texture1DArray.ArraySize = Description.Depth;
					ViewDesc.Texture1DArray.FirstArraySlice = 0;
				}
				break;
			default:
			case CE::ERHIImageDimension::Texture_2D:
				if (Description.Depth == 1)
				{
					ViewDesc.ViewDimension = Description.Samples == ESampleCount::Count_1 ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DMS;
					ViewDesc.Texture2D.MipLevels = Description.MipLevels;
					ViewDesc.Texture2D.MostDetailedMip = Description.MipLevels > 0 ? Description.MipLevels - 1 : 0;
				}
				else
				{
					ViewDesc.ViewDimension = Description.Samples == ESampleCount::Count_1 ? D3D11_SRV_DIMENSION_TEXTURE2DARRAY : D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
					ViewDesc.Texture2DArray.MipLevels = Description.MipLevels;
					ViewDesc.Texture2DArray.MostDetailedMip = 0;
					ViewDesc.Texture2DArray.ArraySize = Description.Depth;
					ViewDesc.Texture2DArray.FirstArraySlice = 0;
				}
				break;
			case CE::ERHIImageDimension::Texture_3D:
				ViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
				ViewDesc.Texture3D.MipLevels = Description.MipLevels;
				ViewDesc.Texture3D.MostDetailedMip = Description.MipLevels > 0 ? Description.MipLevels - 1 : 0;
				break;
			case CE::ERHIImageDimension::Texture_Cube:
				if (Description.Depth == 1)
				{
					ViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					ViewDesc.TextureCube.MipLevels = Description.MipLevels;
					ViewDesc.TextureCube.MostDetailedMip = Description.MipLevels > 0 ? Description.MipLevels - 1 : 0;
				}
				else
				{
					ViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
					ViewDesc.TextureCubeArray.MipLevels = Description.MipLevels;
					ViewDesc.TextureCubeArray.MostDetailedMip = Description.MipLevels > 0 ? Description.MipLevels - 1 : 0;
					ViewDesc.TextureCubeArray.NumCubes = 0;
					ViewDesc.TextureCubeArray.First2DArrayFace = 0;
				}
				break;
			}

			HRESULT Result = m_Owner->Device()->CreateShaderResourceView(m_Image.Get(), &ViewDesc, &m_ShaderResourceView);
			if (Result != S_OK)
			{
				CE_LOG(ELog::Error, "Failed to create Shader Resource View (%s)", DX11_Utilities::ResultToString(Result));
				Destroy();
				return;
			}
		}
	}
}
