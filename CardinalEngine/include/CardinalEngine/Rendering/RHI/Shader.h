#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "Enums.h"
#include "RHIObject.h"

namespace CE
{
	enum class EShaderVariableType
	{
		Unknown,
		Struct,
		Bool,

		Float,
		Float2,
		Float3,
		Float4,
		Float4x4,

		Int,
		Int2,
		Int3,
		Int4,

		UInt,
		UInt2,
		UInt3,
		UInt4,
	};

	struct ShaderReflection_Variable
	{
		EShaderVariableType Type = EShaderVariableType::Unknown;
		String Name;
		uint32 Size = 0;
	};

	struct ShaderReflection_Buffer
	{
		String Name;
		uint32 Size = 0;
		Array<ShaderReflection_Variable> Variables;
	};

	struct ShaderReflection_Image
	{
		String Name;
		ERHIImageDimension Dimension = ERHIImageDimension::Undefined;
		uint32 ArraySize = 0;
	};

	struct ShaderReflection_Sampler
	{
		String Name;
	};

	struct ShaderReflection
	{
		Map<uint32, ShaderReflection_Buffer> Buffers;
		Map<uint32, ShaderReflection_Image> Images;
		Map<uint32, ShaderReflection_Sampler> Samplers;
	};

	class CE_API Shader : public RHIObject
	{
	public:
		virtual EShaderStage Stage() const = 0;

		virtual bool Reflect(ShaderReflection& Result, Array<String>* Errors = nullptr) const = 0;
	};


	class CE_API VertexShader : public Shader
	{
	public:
		virtual EShaderStage Stage() const final override { return EShaderStage::Vertex; }
	};


	class CE_API TessellationControlShader : public Shader
	{
	public:
		virtual EShaderStage Stage() const final override { return EShaderStage::TessellationControl; }
	};


	class CE_API TessellationEvaluationShader : public Shader
	{
	public:
		virtual EShaderStage Stage() const final override { return EShaderStage::TessellationEvaluation; }
	};


	class CE_API GeometryShader : public Shader
	{
	public:
		virtual EShaderStage Stage() const final override { return EShaderStage::Geometry; }
	};


	class CE_API FragmentShader : public Shader
	{
	public:
		virtual EShaderStage Stage() const final override { return EShaderStage::Fragment; }
	};


	class CE_API ComputeShader : public Shader
	{
	public:
		virtual EShaderStage Stage() const final override { return EShaderStage::Compute; }
	};
}
