#include "Inputs.hlsl"

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
};

Material CreateDefaultMaterial(VertexInput Input)
{
    Material Result;
    
    Result.BaseColor = Input.Color;
    Result.Metallic = 0.0;
    Result.Specular = 0.0;
    Result.Roughness = 0.0;
    Result.Anisotropy = 0.0;
    Result.Emissive = float4(0.0, 0.0, 0.0, 1.0);
    Result.Normal = Input.Normal;
    Result.Tangent = Input.Tangent;
    
    return Result;
}

struct Material_InputVertex
{
    float3 Position;
    float3 Normal;
    float2 UV;
    float4 Color;
    float3 Tangent;
    
    float4x4 InstanceTransform;
    float4x4 ModelViewProjection;
    float4 WorldPos;
};

struct Material_InputFragment
{
    float4 Position;
    float3 Normal;
    float2 UV;
    float4 Color;
    float3 Tangent;
    float3 WorldPos;
};

void CalculateMaterial_Vertex(inout Material OutMaterial, Material_InputVertex Input)
{
    
}

void CalculateMaterial_Fragment(inout Material OutMaterial, Material_InputFragment Input)
{
    
}
