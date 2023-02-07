
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
    Result.Roughness = 0.0;
    Result.Anisotropy = 0.0;
    Result.Emissive = float4(0.0, 0.0, 0.0, 1.0);
    Result.Normal = Input.Normal;
    Result.Tangent = Input.Tangent;
        Result.Offset = float3(0.0, 0.0, 0.0);

    return Result;
}

cbuffer Uniform_Enviroment : register(b1)
{
        float3 LightDirection;
};

cbuffer Uniform_Camera : register(b2)
{
        float4x4 ProjectionMatrix;
        float4x4 ViewMatrix;
        float4x4 ProjectionViewMatrix;
        float3 ViewPos;
};

void CalculateMaterial_Vertex(inout Material OutMaterial, Material_InputVertex Input) {}

void CalculateMaterial_Fragment(inout Material OutMaterial, Material_InputFragment Input) {}

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
    Result.Normal = mul((float3x3)MaterialInput.InstanceTransform, Input.Normal);
    Result.UV = Input.UV;
    Result.Color = Input.Color;
    Result.Tangent = Input.Tangent;
        Result.WorldPos = mul(MaterialInput.InstanceTransform, float4(Position, 1.0)).xyz;

        Result.MaterialData = MaterialData;

        return Result;
}

float4 FShader(FragmentInput Input) : SV_TARGET
{
        Material_InputFragment MaterialInput;
    MaterialInput.Position = Input.Position;
    MaterialInput.Normal = Input.Normal;
    MaterialInput.UV = Input.UV;
    MaterialInput.Color = Input.Color;
    MaterialInput.Tangent = Input.Tangent;
    MaterialInput.WorldPos = Input.WorldPos;

        Material MaterialData = Input.MaterialData;
        CalculateMaterial_Fragment(MaterialData, MaterialInput);

        return MaterialData.BaseColor;
}