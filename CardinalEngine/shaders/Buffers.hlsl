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
