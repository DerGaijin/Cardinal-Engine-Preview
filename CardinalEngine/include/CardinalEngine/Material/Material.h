#pragma once
#include "../Framework/Core/Headers.h"
#include "../AssetSystem/Asset.h"
#include "../AssetSystem/AssetRef.h"
#include "Texture.h"

#include "../Rendering/RHI/Shader.h"
#include "../Rendering/RHI/Pipeline.h"

namespace CE
{
	class RHIContextImmediate;
	class RHIImage;
	class Sampler;
	class RHIBuffer;

	enum class EMaterialDomain
	{
		Geometry,
		//GUI
		//PostProcess
	};

	enum class EMaterialBlend
	{
		Opaque,
		Translucent,
	};

	enum class EMaterialShading
	{
		Unlit,
		DefaultLit,
	};

	struct MaterialSettings
	{
		EMaterialDomain Domain = EMaterialDomain::Geometry;
		EMaterialBlend Blend = EMaterialBlend::Opaque;
		EMaterialShading Shading = EMaterialShading::DefaultLit;

		bool TwoSided = false;
		bool Wireframe = false;

		EShaderSourceLanguage ShaderLanguage = EShaderSourceLanguage::HLSL;
		String ShaderSource;
	};


	enum class EMaterialBufferType
	{
		Frame,
		Environment,
		Camera,
		StaticParameters,
		DynamicParameters,

		_COUNT,
	};


	class CE_API Material : public Asset
	{
		CE_OBJECT_BODY()
	public:
		struct SamplerInfo
		{
			uint32 Binding;
			ShaderReflection_Sampler Reflection;
			SharedPtr<Sampler> Sampler;
		};

		struct TextureInfo
		{
			uint32 Binding;
			ShaderReflection_Image Reflection;
			AssetRef<Texture> Texture;
		};

	public:
		Material() = default;

		Material(const MaterialSettings& Settings, Array<String>* Errors = nullptr);

		bool IsValid() const;

		bool SetParameter(const String& Name, const bool& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, bool& Value) const;

		bool SetParameter(const String& Name, const float& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, float& Value) const;

		bool SetParameter(const String& Name, const Vec2f& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, Vec2f& Value) const;

		bool SetParameter(const String& Name, const Vec3f& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, Vec3f& Value) const;

		bool SetParameter(const String& Name, const Vec4f& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, Vec4f& Value) const;

		bool SetParameter(const String& Name, const Mat4x4f& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, Mat4x4f& Value) const;

		bool SetParameter(const String& Name, const int32& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, int32& Value) const;

		bool SetParameter(const String& Name, const Vec2i& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, Vec2i& Value) const;

		bool SetParameter(const String& Name, const Vec3i& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, Vec3i& Value) const;

		bool SetParameter(const String& Name, const Vec4i& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, Vec4i& Value) const;

		bool SetParameter(const String& Name, const uint32& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, uint32& Value) const;

		bool SetParameter(const String& Name, const Vec2ui& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, Vec2ui& Value) const;

		bool SetParameter(const String& Name, const Vec3ui& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, Vec3ui& Value) const;

		bool SetParameter(const String& Name, const Vec4ui& Value, bool UploadToGPU = true);
		bool GetParameter(const String& Name, Vec4ui& Value) const;

		bool SetParameter(const String& Name, const char* Value, size_t ValueSize, bool UploadToGPU = true);
		bool GetParameter(const String& Name, char* Value, size_t ValueSize) const;

		void UploadParametersToGPU();

	public:
		const MaterialSettings& Settings() const
		{
			return m_Settings;
		}

		const SharedPtr<RHIPipeline_Graphic>& PBRPipeline() const
		{
			return m_PBRPipeline;
		}

		const SharedPtr<RHIPipeline_Graphic>& DepthPipeline() const
		{
			return m_DepthPipeline;
		}

		const Array<ShaderReflection_Variable>& DynamicParameters() const
		{
			return m_DynamicParamBufferDesc.Variables;
		}

		const Array<char>& DynamicParameterValues() const
		{
			return m_DynamicParamValues;
		}

		int32 BufferBinding(EMaterialBufferType Type) const
		{
			return m_BufferBindings[(int)Type];
		}

		const SharedPtr<RHIBuffer>& DynamicParameterBuffer() const
		{
			return m_DynamicParamBuffer;
		}

		const Map<String, SamplerInfo>& Samplers() const
		{
			return m_Samplers;
		}

		const Map<String, TextureInfo>& Textures() const
		{
			return m_Textures;
		}

	protected:
		void SerializeAsset(Archive& Ar) override;

	private:
		String LoadShaderCode(const std::filesystem::path& Path) const;

		bool CreateShaders(RHIPipeline_GraphicShaders& Shaders, bool IsDepth, Array<String>* Errors) const;
		bool ReflectShaders(const RHIPipeline_GraphicShaders& Shaders, ShaderReflection& Reflection, std::array<int32, (int)EMaterialBufferType::_COUNT>& BufferBindings, ShaderReflection_Buffer& DynamicParameterBuffer, Map<String, SamplerInfo>& Samplers, Map<String, TextureInfo>& Textures, Array<String>* Errors);
		void CreatePipelineDescription(RHIPipeline_GraphicDescription& Description, const std::array<int32, (int)EMaterialBufferType::_COUNT>& BufferBindings, bool IsDepth, Array<String>* Errors) const;
		bool CreateParameterBuffer(SharedPtr<RHIBuffer>& ParameterBuffer, Array<char>& ParameterValues, int32 Size, Array<String>* Errors);

		void ReconstructMaterial(Array<String>* Errors);

	private:
		MaterialSettings m_Settings;

		SharedPtr<RHIPipeline_Graphic> m_PBRPipeline;
		SharedPtr<RHIPipeline_Graphic> m_DepthPipeline;

		ShaderReflection_Buffer m_DynamicParamBufferDesc;
		SharedPtr<RHIBuffer> m_DynamicParamBuffer;
		Array<char> m_DynamicParamValues;

		std::array<int32, (int)EMaterialBufferType::_COUNT> m_BufferBindings;
		Map<String, SamplerInfo> m_Samplers;
		Map<String, TextureInfo> m_Textures;
	};
}
