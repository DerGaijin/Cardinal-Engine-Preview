#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"

#include "DirectX11/D3D11.h"
#include "DirectX11/D3Dcompiler.h"

namespace CE
{
	class DX11_Functions
	{
	public:
		DX11_Functions() = delete;

		using D3D11CreateDevice_t = HRESULT(*)(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext);
		static D3D11CreateDevice_t D3D11CreateDevice;

		using D3DCompile_t = HRESULT(*)(LPCVOID pSrcData, SIZE_T srcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs);
		static D3DCompile_t D3DCompile;

		using D3DReflect_t = HRESULT(*)(LPCVOID pSrcData, SIZE_T SrcDataSize, REFIID pInterface, void** ppReflector);
		static D3DReflect_t D3DReflect;

		using D3DCreateBlob_t = HRESULT(*)(SIZE_T Size, ID3DBlob** ppBlob);
		static D3DCreateBlob_t D3DCreateBlob;
	};
}
