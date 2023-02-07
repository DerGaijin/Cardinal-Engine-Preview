#pragma once
#include "../../Framework/Core/Headers.h"
#include "RHIObject.h"
#include "Enums.h"

namespace CE
{
	class VertexShader;
	class TessellationControlShader;
	class TessellationEvaluationShader;
	class GeometryShader;
	class FragmentShader;
	class ComputeShader;

	class CE_API RHIPipeline : public RHIObject
	{
	public:
	

	};



	struct ShaderResourceBinding
	{
		ShaderResourceBinding() = default;
		ShaderResourceBinding(uint32 Binding, EShaderResourceType Type) : Binding(Binding), Type(Type) {}
		uint32 Binding = 0;
		EShaderResourceType Type;
	};

	struct ShaderVertexInput
	{
		ShaderVertexInput(uint32 Index, EDataFormat Format, uint32 RelativeOffset = 0, uint32 BufferIndex = 0, bool PerInstance = false)
			: Index(Index), Format(Format), RelativeOffset(RelativeOffset), BufferIndex(BufferIndex), PerInstance(PerInstance) {}
		uint32 Index = 0;
		EDataFormat Format = EDataFormat::Unknown;
		uint32 RelativeOffset = 0;
		uint32 BufferIndex = 0;
		bool PerInstance = false;
	};

	struct RenderTargetBlend
	{
		bool BlendEnabled = false;
		bool LogicOperationEnabled = false;
		EBlendFactor SrcBlend = EBlendFactor::One;
		EBlendFactor DstBlend = EBlendFactor::Zero;
		EBlendOperation BlendOp = EBlendOperation::Add;
		EBlendFactor SrcBlendAlpha = EBlendFactor::One;
		EBlendFactor DstBlendAlpha = EBlendFactor::Zero;
		EBlendOperation BlendOpAlpha = EBlendOperation::Add;
		ELogicOperation LogicOp = ELogicOperation::Copy;
		EBlendMask WriteMask = EBlendMask::AllComponents;
	};

	struct RHIPipeline_GraphicShaders
	{
		SharedPtr<VertexShader> VertexShader;
		SharedPtr<TessellationControlShader> TessellationControlShader;
		SharedPtr<TessellationEvaluationShader> TessellationEvaluationShader;
		SharedPtr<GeometryShader> GeometryShader;
		SharedPtr<FragmentShader> FragmentShader;
	};

	struct RHIPipeline_GraphicDescription
	{
		Array<EDataFormat> RenderTargetFormats;
		EDataFormat DepthStencilFormat = EDataFormat::Unknown;

		Array<ShaderVertexInput> VertexInputLayout;
		Array<ShaderResourceBinding> ResourceBindings;

		EPrimitiveTopology Topology = EPrimitiveTopology::TriangleList;

		struct Rasterizer_t
		{
			bool WireFrame = false;
			ECullMode CullMode = ECullMode::Back;
			bool FrontIsCounterClockwise = false;
			bool ScissorEnabled = false;
			bool DepthClampEnabled = false;
			bool DepthBiasEnabled = false;
			int DepthBias = 0;
			float DepthBiasClamp = 0.0f;
			float DepthBiasSlopeFactor = 0.0f;
		} Rasterizer;

		struct BlendState_t
		{
			bool AlphaToCoverageEnabled = false;
			bool IndependetBlendEnabled = false;
			Array<RenderTargetBlend> RenderTargetBlends;
		} BlendState;

		struct DepthStencilState_t
		{
			bool DepthEnabled = true;
			bool DepthWriteEnabled = true;
			ECompareOp DepthCompare = ECompareOp::Less;
			bool StencilEnabled = false;

			bool ReadMaskAll = true;
			bool WriteMaskAll = true;

			EStencilOp FrontStencilFailOp = EStencilOp::Keep;
			EStencilOp FrontStencilDepthFailOp = EStencilOp::IncrWrap;
			EStencilOp FrontStencilPassOp = EStencilOp::Keep;
			ECompareOp FrontStencilOp = ECompareOp::Always;

			EStencilOp BackStencilFailOp = EStencilOp::Keep;
			EStencilOp BackStencilDepthFailOp = EStencilOp::DecrWrap;
			EStencilOp BackStencilPassOp = EStencilOp::Keep;
			ECompareOp BackStencilOp = ECompareOp::Always;
		} DepthStencilState;

		struct MultisampleState_t
		{
			ESampleCount Count = ESampleCount::Count_1;
			uint8_t Quality = 0;
		} MultisampleState;
	};
	class CE_API RHIPipeline_Graphic : public RHIPipeline
	{
	public:
		RHIPipeline_Graphic(const RHIPipeline_GraphicDescription& Desc) : Description(Desc) {}



	public:
		const RHIPipeline_GraphicDescription Description;
	};



	class CE_API RHIPipeline_Compute : public RHIPipeline
	{
	public:


	};



	class CE_API RHIPipeline_Raytracing : public RHIPipeline
	{
	public:


	};
}
