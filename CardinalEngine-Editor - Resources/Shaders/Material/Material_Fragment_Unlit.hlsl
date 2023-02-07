#ifndef __MATERIAL_FRAGMENT_UNLIT__
#define __MATERIAL_FRAGMENT_UNLIT__

#if !CE_BUILD_MATERIAL
#include "Material_Data.hlsl"
#include "Material_Inputs.hlsl"
#include "Material_Buffers.hlsl"
#endif

float4 FShader(FragmentInput Input) : SV_TARGET
{
    Material_InputFragment MaterialInput;
    MaterialInput.Position = Input.Position.xyz;
    MaterialInput.Normal = Input.Normal;
    MaterialInput.UV = Input.UV;
    MaterialInput.Color = Input.Color;
    MaterialInput.Tangent = Input.Tangent;
    MaterialInput.WorldPos = Input.WorldPos;

    Material MaterialData = Input.MaterialData;
    CalculateMaterial_Fragment(MaterialData, MaterialInput);

    return MaterialData.BaseColor;
}

#endif
