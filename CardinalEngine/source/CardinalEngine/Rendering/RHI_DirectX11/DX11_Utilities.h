#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Rendering/RHI/Enums.h"

#include "DirectX11/d3d11.h"

namespace CE
{
	class DX11_Utilities
	{
	public:
		DX11_Utilities() = delete;

		static const char* ResultToString(HRESULT Result);

		static DXGI_FORMAT TranslateDataFormat(EDataFormat Format);

		static D3D11_COMPARISON_FUNC TranslateCompareOp(ECompareOp Op);

		static D3D11_STENCIL_OP TranslateStencilOp(EStencilOp Op);
	};
}
