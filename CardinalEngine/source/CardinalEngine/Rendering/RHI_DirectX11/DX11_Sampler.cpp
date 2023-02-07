#include "DX11_Sampler.h"
#include "DX11_DynamicRHI.h"

#include "DX11_Utilities.h"

namespace CE
{
	DX11_Sampler::DX11_Sampler(DX11_DynamicRHI* Owner, const SamplerDescription& InDescription)
		: m_Owner(Owner), Sampler(InDescription)
	{
		D3D11_SAMPLER_DESC SamplerDesc{};
		if (Description.MinFilter == ESamplerFilter::Nearest && Description.MagFilter == ESamplerFilter::Nearest && Description.MipMapMode == ESamplerFilter::Nearest)
			SamplerDesc.Filter = Description.EnableCompare ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_POINT;
		if (Description.MinFilter == ESamplerFilter::Nearest && Description.MagFilter == ESamplerFilter::Nearest && Description.MipMapMode == ESamplerFilter::Linear)
			SamplerDesc.Filter = Description.EnableCompare ? D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR : D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		if (Description.MinFilter == ESamplerFilter::Nearest && Description.MagFilter == ESamplerFilter::Linear && Description.MipMapMode == ESamplerFilter::Nearest)
			SamplerDesc.Filter = Description.EnableCompare ? D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT : D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		if (Description.MinFilter == ESamplerFilter::Nearest && Description.MagFilter == ESamplerFilter::Linear && Description.MipMapMode == ESamplerFilter::Linear)
			SamplerDesc.Filter = Description.EnableCompare ? D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR : D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		if (Description.MinFilter == ESamplerFilter::Linear && Description.MagFilter == ESamplerFilter::Nearest && Description.MipMapMode == ESamplerFilter::Nearest)
			SamplerDesc.Filter = Description.EnableCompare ? D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT : D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		if (Description.MinFilter == ESamplerFilter::Linear && Description.MagFilter == ESamplerFilter::Nearest && Description.MipMapMode == ESamplerFilter::Linear)
			SamplerDesc.Filter = Description.EnableCompare ? D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR : D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		if (Description.MinFilter == ESamplerFilter::Linear && Description.MagFilter == ESamplerFilter::Linear && Description.MipMapMode == ESamplerFilter::Nearest)
			SamplerDesc.Filter = Description.EnableCompare ? D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT : D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		if (Description.MinFilter == ESamplerFilter::Linear && Description.MagFilter == ESamplerFilter::Linear && Description.MipMapMode == ESamplerFilter::Linear)
			SamplerDesc.Filter = Description.EnableCompare ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		static auto TranslateAddressMode = [](ESamplerAddressMode Mode) -> D3D11_TEXTURE_ADDRESS_MODE
		{
			switch (Mode)
			{
			default:
			case CE::ESamplerAddressMode::Repeat:
				return D3D11_TEXTURE_ADDRESS_WRAP;
			case CE::ESamplerAddressMode::MirroredRepeat:
				return D3D11_TEXTURE_ADDRESS_MIRROR;
			case CE::ESamplerAddressMode::ClampToEdge:
				return D3D11_TEXTURE_ADDRESS_CLAMP;
			case CE::ESamplerAddressMode::ClampToBorder:
				return D3D11_TEXTURE_ADDRESS_BORDER;
			}
		};

		SamplerDesc.AddressU = TranslateAddressMode(Description.AddressModeU);
		SamplerDesc.AddressV = TranslateAddressMode(Description.AddressModeV);
		SamplerDesc.AddressW = TranslateAddressMode(Description.AddressModeW);
		SamplerDesc.MipLODBias = Description.MipLODBias;
		SamplerDesc.MaxAnisotropy = (UINT)Description.MaxAnisotropy;	//@TODO: Vulkan uses float, maybe we use int for every API
		SamplerDesc.ComparisonFunc = DX11_Utilities::TranslateCompareOp(Description.CompareOp);
		switch (Description.Border)
		{
		default:
		case ESamplerBorder::TransparentBlack:
			SamplerDesc.BorderColor[0] = 0.0f;
			SamplerDesc.BorderColor[1] = 0.0f;
			SamplerDesc.BorderColor[2] = 0.0f;
			SamplerDesc.BorderColor[3] = 0.0f;
			break;
		case ESamplerBorder::OpaqueBlack:
			SamplerDesc.BorderColor[0] = 0.0f;
			SamplerDesc.BorderColor[1] = 0.0f;
			SamplerDesc.BorderColor[2] = 0.0f;
			SamplerDesc.BorderColor[3] = 1.0f;
			break;
		case ESamplerBorder::OpaqueWhite:
			SamplerDesc.BorderColor[0] = 1.0f;
			SamplerDesc.BorderColor[1] = 1.0f;
			SamplerDesc.BorderColor[2] = 1.0f;
			SamplerDesc.BorderColor[3] = 1.0f;
			break;
		}
		SamplerDesc.MinLOD = Description.MinLOD;
		SamplerDesc.MaxLOD = Description.MaxLOD;

		HRESULT Result = m_Owner->Device()->CreateSamplerState(&SamplerDesc, &m_Sampler);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to Create Sampler (%s)", DX11_Utilities::ResultToString(Result));
			return;
		}
	}

	bool DX11_Sampler::IsValid() const
	{
		return m_Sampler != nullptr;
	}

	DynamicRHI* DX11_Sampler::Owner() const
	{
		return m_Owner;
	}

	void DX11_Sampler::Destroy()
	{
		m_Sampler = nullptr;
	}
}
