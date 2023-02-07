#ifndef __MATERIAL_DATA__
#define __MATERIAL_DATA__

struct Material
{
    float4 BaseColor;
    float Metallic;
    float Specular;
    float Roughness;
    float Anisotropy;
    float4 Emissive;

    float3 Normal;
    float3 Tangent;

    float3 Offset;
};

struct Material_InputVertex
{
    float3 Position;
    float3 Normal;
    float2 UV;
    float4 Color;
    float3 Tangent;
    
    float4x4 InstanceTransform;
    float4x4 ModelViewProjection;
    float3 WorldPos;
};

struct Material_InputFragment
{
    float3 Position;
    float3 Normal;
    float2 UV;
    float4 Color;
    float3 Tangent;
    float3 WorldPos;
};

#endif
