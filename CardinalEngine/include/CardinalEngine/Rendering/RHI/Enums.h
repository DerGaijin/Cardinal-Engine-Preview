#pragma once
#include "../../Framework/Core/Headers.h"

namespace CE
{
	enum class ERHIType
	{
		None,
		Vulkan,
		DirectX12,
		DirectX11,
	};

	enum class EDataFormat
	{
		Unknown,

		RGBA32_Typeless,
		RGBA32_Float,
		RGBA32_UInt,
		RGBA32_Int,

		RGB32_Typeless,
		RGB32_Float,
		RGB32_UInt,
		RGB32_Int,

		RGBA16_Typeless,
		RGBA16_Float,
		RGBA16_UNorm,
		RGBA16_UInt,
		RGBA16_SNorm,
		RGBA16_Int,

		RG32_Typeless,
		RG32_Float,
		RG32_UInt,
		RG32_Int,

		RGBA8_Typeless,
		RGBA8_UNorm,
		RGBA8_UNorm_SRGB,
		RGBA8_UInt,
		RGBA8_SNorm,
		RGBA8_Int,

		BGRA8_UNorm,
		BGRA8_UNorm_SRGB,
		BGRA8_SNorm,
		BGRA8_UInt,
		BGRA8_Int,

		RG16_Typeless,
		RG16_Float,
		RG16_UNorm,
		RG16_UInt,
		RG16_SNorm,
		RG16_Int,

		R32_Typeless,
		D32_Float,
		R32_Float,
		R32_UInt,
		R32_Int,

		RG8_Typeless,
		RG8_UNorm,
		RG8_UInt,
		RG8_SNorm,
		RG8_Int,

		R16_Typeless,
		R16_Float,
		D16_UNorm,
		R16_UNorm,
		R16_UInt,
		R16_SNorm,
		R16_Int,

		R8_Typeless,
		R8_UNorm,
		R8_UInt,
		R8_SNorm,
		R8_Int,
		A8_UNorm,
	};

	enum class EShaderStage
	{
		Vertex = 0,
		TessellationControl,
		TessellationEvaluation,
		Geometry,
		Fragment,
		Compute,
	};

	enum class EShaderSourceLanguage
	{
		HLSL = 0,
		GLSL,
	};

	enum class EShaderResourceType
	{
		UniformBuffer,
		SampledImage,
		Sampler,
	};

	enum class ERHIBufferUsage
	{
		None = 0,
		VertexBuffer,
		IndexBuffer,
		UniformBuffer,
	};

	enum class ERHIImageUsage
	{
		None = 0,
		ShaderResource = CE_ENUM_BIT(0),
		RenderTarget = CE_ENUM_BIT(1),
		DepthStencil = CE_ENUM_BIT(2),
	};
	CE_ENUM_BIT_OPERATORS(ERHIImageUsage);

	enum class ERHIAccess
	{
		GPUOnly = 0,
		GPURead_CPUWrite,
		All,
	};

	enum class ERHIMapAccess
	{
		Read,
		Write,
		ReadWrite,
	};

	enum class EIndexBufferType
	{
		UInt16,
		UInt32,
	};

	enum class ERHIImageDimension
	{
		Undefined = 0,
		Texture_1D,
		Texture_2D,
		Texture_3D,
		Texture_Cube,
	};

	enum class ESampleCount
	{
		Count_1 = 1,
		Count_2 = 2,
		Count_4 = 4,
		Count_8 = 8,
		Count_16 = 16,
		Count_32 = 32,
		Count_64 = 64,
	};

	enum class ECompareOp
	{
		Never = 0,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreaterOrEqual,
		Always,
	};

	enum class EBlendFactor
	{
		Undefined = 0,
		Zero,
		One,
		SrcColor,
		InvSrcColor,
		SrcAlpha,
		InvSrcAlpha,
		DstAlpha,
		InvDstAlpha,
		DstColor,
		InvDstColor,
		SrcAlphaSat,
		BlendFactor,
		InvBlendFactor,
		Src1Color,
		InvSrc1Color,
		Src1Alpha,
		InvSrc1Alpha,
	};

	enum class EBlendOperation
	{
		Undefined = 0,
		Add,
		Subtract,
		RevSubtract,
		Min,
		Max,
	};

	enum class ELogicOperation
	{
		Clear = 0,
		Set,
		Copy,
		CopyInverted,
		NoOp,
		Invert,
		And,
		NAnd,
		Or,
		NOr,
		XOr,
		Equiv,
		AndReverse,
		AndInverted,
		OrReverse,
		OrInverted,
	};

	enum class EBlendMask
	{
		None = 0,
		Component_R = 1 << 0,
		Component_G = 1 << 1,
		Component_B = 1 << 2,
		Component_A = 1 << 3,
		AllComponents = Component_R | Component_G | Component_B | Component_A,
	};
	CE_ENUM_BIT_OPERATORS(EBlendMask);

	enum class EPrimitiveTopology
	{
		TriangleList = 0,
		TriangleStrip,
		PointList,
		LineList,
		LineStrip,
	};

	enum class ECullMode
	{
		None,
		Back,
		Front,
	};

	enum class EStencilOp
	{
		Undefined = 0,
		Keep,
		Zero,
		Replace,
		IncrSat,
		DecrSat,
		Invert,
		IncrWrap,
		DecrWrap,
	};

	enum class ESamplerFilter
	{
		Nearest = 0,
		Linear,
	};

	enum class ESamplerAddressMode
	{
		Repeat = 0,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
	};

	enum class ESamplerBorder
	{
		TransparentBlack = 0,
		OpaqueBlack,
		OpaqueWhite,
	};
}
