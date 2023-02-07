#include "DX11_Functions.h"

namespace CE
{
	CE::DX11_Functions::D3D11CreateDevice_t DX11_Functions::D3D11CreateDevice = nullptr;
	CE::DX11_Functions::D3DCompile_t DX11_Functions::D3DCompile = nullptr;
	CE::DX11_Functions::D3DReflect_t DX11_Functions::D3DReflect = nullptr;
	CE::DX11_Functions::D3DCreateBlob_t DX11_Functions::D3DCreateBlob = nullptr;
}
