#ifndef __MATERIAL_FRAGMENT_UNLIT__
#define __MATERIAL_FRAGMENT_UNLIT__

#if !CE_BUILD_MATERIAL
#include "Material_Data.hlsl"
#include "Material_Inputs.hlsl"
#include "Material_Buffers.hlsl"
#endif

static const float PI = 3.14159265f;
static const float Epsilon = 0.000001;


float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;
    return a2 / max(denom, Epsilon);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
    float ggx2 = NdotL / (NdotL * (1.0 - k) + k);
    return ggx1 * ggx2;
}

float3 FresnelSchlick(float HdotV, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}

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

    float3 LightDir = float3(0.5, -1.0, 1.0);
	
    float3 albedo = MaterialData.BaseColor.xyz;
    float3 Emissive = MaterialData.Emissive.xyz;
    float roughness = max(MaterialData.Roughness, 0.05);
    float metalness = MaterialData.Metallic;
	
    float3 F0 = lerp(0.04, albedo, metalness);
	
    float3 N = normalize(MaterialData.Normal);
    float3 V = normalize(ViewPos - Input.WorldPos);
	
	// Per Light Begin
    float3 L = normalize(LightDir);
    float3 H = normalize(V + L);
	
    float NdotH = max(dot(N, H), Epsilon);
    float NdotV = max(dot(N, V), Epsilon);
    float NdotL = max(dot(N, L), Epsilon);
    float HdotV = max(dot(H, V), Epsilon);
	
    float D = DistributionGGX(NdotH, roughness);
    float G = GeometrySmith(NdotV, NdotL, roughness);
    float3 F = FresnelSchlick(HdotV, F0);
	
    float3 Specular = D * G * F;
    Specular /= 4.0 * NdotV * NdotL;
	
    float3 kD = 1.0 - F;
    kD *= 1.0 - metalness;
	
    float3 Lo = (kD * albedo / PI + Specular) * 1.0 * NdotL;
	// Per Light End   
	
    float3 Ambient = 0.03 * albedo;
	
    float3 Color = Ambient + Lo;
	
	// HDR tonemapping
    Color = Color / (Color + 1.0);
	
	// Gamma correct
    Color = pow(abs(Color), 1.0 / 2.2);
	
    return float4(Color, MaterialData.BaseColor.w);
}

#endif
