#ifndef __MATERIAL_VERTEX__
#define __MATERIAL_VERTEX__

#if !CE_BUILD_MATERIAL
#include "Material_Data.hlsl"
#include "Material_Inputs.hlsl"
#include "Material_Buffers.hlsl"
#endif

FragmentInput VShader(VertexInput Input, InstanceInput Instance)
{
    Material_InputVertex MaterialInput;
    MaterialInput.Position = Input.Position;
    MaterialInput.Normal = Input.Normal;
    MaterialInput.UV = Input.UV;
    MaterialInput.Color = Input.Color;
    MaterialInput.Tangent = Input.Tangent;

    MaterialInput.InstanceTransform = float4x4(
		float4(Instance.Transfrom_Row0.x, Instance.Transfrom_Row1.x, Instance.Transfrom_Row2.x, Instance.Transfrom_Row3.x),
		float4(Instance.Transfrom_Row0.y, Instance.Transfrom_Row1.y, Instance.Transfrom_Row2.y, Instance.Transfrom_Row3.y),
    	float4(Instance.Transfrom_Row0.z, Instance.Transfrom_Row1.z, Instance.Transfrom_Row2.z, Instance.Transfrom_Row3.z),
		float4(Instance.Transfrom_Row0.w, Instance.Transfrom_Row1.w, Instance.Transfrom_Row2.w, Instance.Transfrom_Row3.w)
	);

    MaterialInput.ModelViewProjection = mul(ProjectionViewMatrix, MaterialInput.InstanceTransform);
    MaterialInput.WorldPos = mul(MaterialInput.InstanceTransform, float4(Input.Position, 1.0)).xyz;

    Material MaterialData = CreateDefaultMaterial(Input);
    CalculateMaterial_Vertex(MaterialData, MaterialInput);

    float3 Position = Input.Position + MaterialData.Offset;

    FragmentInput Result;
    Result.Position = mul(MaterialInput.ModelViewProjection, float4(Position, 1.0));
    Result.Normal = mul((float3x3) MaterialInput.InstanceTransform, Input.Normal);
    Result.UV = Input.UV;
    Result.Color = Input.Color;
    Result.Tangent = Input.Tangent;
    Result.WorldPos = mul(MaterialInput.InstanceTransform, float4(Position, 1.0)).xyz;

    Result.MaterialData = MaterialData;

    return Result;
}

#endif
