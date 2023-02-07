#ifndef __MATERIAL_FRAGMENT_DEPTH__
#define __MATERIAL_FRAGMENT_DEPTH__

#if !CE_BUILD_MATERIAL
#include "Material_Data.hlsl"
#include "Material_Inputs.hlsl"
#include "Material_Buffers.hlsl"
#endif

float4 FShader(FragmentInput Input) : SV_TARGET
{
    return Input.Color;
}

#endif
