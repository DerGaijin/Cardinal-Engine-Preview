#include "DX11_Utilities.h"

namespace CE
{
	const char* DX11_Utilities::ResultToString(HRESULT Result)
	{
		switch (Result)
		{
		case S_OK:
			return "Success";
		case E_INVALIDARG:
			return "One or more Arguments are invalid";
		}
		return "UNKNOWN";
	}

	DXGI_FORMAT DX11_Utilities::TranslateDataFormat(EDataFormat Format)
	{
		switch (Format)
		{
		case CE::EDataFormat::Unknown:
			return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		case CE::EDataFormat::RGBA32_Typeless:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_TYPELESS;
		case CE::EDataFormat::RGBA32_Float:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		case CE::EDataFormat::RGBA32_UInt:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT;
		case CE::EDataFormat::RGBA32_Int:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT;
		case CE::EDataFormat::RGB32_Typeless:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_TYPELESS;
		case CE::EDataFormat::RGB32_Float:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		case CE::EDataFormat::RGB32_UInt:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT;
		case CE::EDataFormat::RGB32_Int:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_SINT;
		case CE::EDataFormat::RGBA16_Typeless:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_SINT;
		case CE::EDataFormat::RGBA16_Float:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
		case CE::EDataFormat::RGBA16_UNorm:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UNORM;
		case CE::EDataFormat::RGBA16_UInt:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UINT;
		case CE::EDataFormat::RGBA16_SNorm:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SNORM;
		case CE::EDataFormat::RGBA16_Int:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SINT;
		case CE::EDataFormat::RG32_Typeless:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32_TYPELESS;
		case CE::EDataFormat::RG32_Float:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
		case CE::EDataFormat::RG32_UInt:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT;
		case CE::EDataFormat::RG32_Int:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32_SINT;
		case CE::EDataFormat::RGBA8_Typeless:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_TYPELESS;
		case CE::EDataFormat::RGBA8_UNorm:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		case CE::EDataFormat::RGBA8_UNorm_SRGB:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case CE::EDataFormat::RGBA8_UInt:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT;
		case CE::EDataFormat::RGBA8_SNorm:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SNORM;
		case CE::EDataFormat::RGBA8_Int:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SINT;
		case CE::EDataFormat::RG16_Typeless:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16_TYPELESS;
		case CE::EDataFormat::RG16_Float:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16_FLOAT;
		case CE::EDataFormat::RG16_UNorm:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16_UNORM;
		case CE::EDataFormat::RG16_UInt:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16_UINT;
		case CE::EDataFormat::RG16_SNorm:
			return DXGI_FORMAT::DXGI_FORMAT_R16G16_SNORM;

		case CE::EDataFormat::RG16_Int:
			break;

		case CE::EDataFormat::R32_Typeless:
			return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		case CE::EDataFormat::D32_Float:
			return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		case CE::EDataFormat::R32_Float:
			return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		case CE::EDataFormat::R32_UInt:
			return DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
		case CE::EDataFormat::R32_Int:
			return DXGI_FORMAT::DXGI_FORMAT_R32_SINT;

		case CE::EDataFormat::RG8_Typeless:

		case CE::EDataFormat::RG8_UNorm:

		case CE::EDataFormat::RG8_UInt:

		case CE::EDataFormat::RG8_SNorm:

		case CE::EDataFormat::RG8_Int:

		case CE::EDataFormat::R16_Typeless:

		case CE::EDataFormat::R16_Float:

		case CE::EDataFormat::D16_UNorm:

		case CE::EDataFormat::R16_UNorm:

		case CE::EDataFormat::R16_UInt:

		case CE::EDataFormat::R16_SNorm:

		case CE::EDataFormat::R16_Int:

		case CE::EDataFormat::R8_Typeless:

		case CE::EDataFormat::R8_UNorm:

		case CE::EDataFormat::R8_UInt:

		case CE::EDataFormat::R8_SNorm:

		case CE::EDataFormat::R8_Int:

		case CE::EDataFormat::A8_UNorm:

		default:
			break;
		}
		CE_LOG(ELog::Warning, "Failed to Translate Format");
		return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	}

	D3D11_COMPARISON_FUNC DX11_Utilities::TranslateCompareOp(ECompareOp Op)
	{
		switch (Op)
		{
		case ECompareOp::Never: return D3D11_COMPARISON_NEVER;
		case ECompareOp::Less: return D3D11_COMPARISON_LESS;
		case ECompareOp::Equal: return D3D11_COMPARISON_EQUAL;
		case ECompareOp::LessOrEqual: return D3D11_COMPARISON_LESS_EQUAL;
		case ECompareOp::Greater: return D3D11_COMPARISON_GREATER;
		case ECompareOp::NotEqual: return D3D11_COMPARISON_NOT_EQUAL;
		case ECompareOp::GreaterOrEqual: return D3D11_COMPARISON_GREATER_EQUAL;
		case ECompareOp::Always: return D3D11_COMPARISON_ALWAYS;
		}
		CE_LOG(ELog::Warning, "Failed to translate ECompareOp");
		return D3D11_COMPARISON_NEVER;
	}

	D3D11_STENCIL_OP DX11_Utilities::TranslateStencilOp(EStencilOp Op)
	{
		switch (Op)
		{
		case EStencilOp::Keep: return D3D11_STENCIL_OP_KEEP;
		case EStencilOp::Zero: return D3D11_STENCIL_OP_ZERO;
		case EStencilOp::Replace: return D3D11_STENCIL_OP_REPLACE;
		case EStencilOp::IncrSat: return D3D11_STENCIL_OP_INCR_SAT;
		case EStencilOp::DecrSat: return D3D11_STENCIL_OP_DECR_SAT;
		case EStencilOp::Invert: return D3D11_STENCIL_OP_INVERT;
		case EStencilOp::IncrWrap: return D3D11_STENCIL_OP_INCR;
		case EStencilOp::DecrWrap: return D3D11_STENCIL_OP_DECR;
		}
		CE_LOG(ELog::Warning, "Failed to translate EStencilOp");
		return D3D11_STENCIL_OP_ZERO;
	}
}
