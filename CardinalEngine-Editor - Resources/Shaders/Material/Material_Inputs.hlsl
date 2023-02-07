#ifndef __MATERIAL_INPUTS__
#define __MATERIAL_INPUTS__

#if !CE_BUILD_MATERIAL
#include "Material_Data.hlsl"
#endif

struct VertexInput
{
    float3 Position : ATTRIB0;
    float3 Normal : ATTRIB1;
    float2 UV : ATTRIB2;
    float4 Color : ATTRIB3;
    float3 Tangent : ATTRIB4;
};

struct InstanceInput
{
    float4 Transfrom_Row0 : ATTRIB5;
    float4 Transfrom_Row1 : ATTRIB6;
    float4 Transfrom_Row2 : ATTRIB7;
    float4 Transfrom_Row3 : ATTRIB8;
};

struct FragmentInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 UV : UVCOORD;
    float4 Color : COLOR;
    float3 Tangent : TANGENT;
    float3 WorldPos : WORLDPOS;

    Material MaterialData : MATERIALDATA;
};

Material CreateDefaultMaterial(VertexInput Input)
{
    Material Result;
    
    Result.BaseColor = Input.Color;
    Result.Metallic = 0.0;
    Result.Specular = 0.0;
    Result.Roughness = 1.0;
    Result.Anisotropy = 0.0;
    Result.Emissive = float4(0.0, 0.0, 0.0, 1.0);
    Result.Normal = Input.Normal;
    Result.Tangent = Input.Tangent;
    Result.Offset = float3(0.0, 0.0, 0.0);
    
    return Result;
}

#endif
