#include "CardinalEngine/Material/Material.h"
#include "CardinalEngine/Framework/Serialization/Archive.h"
#include "CardinalEngine/Framework/Serialization/AdditionalOperators.h"
#include "CardinalEngine/Mesh/MeshVertex.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"
#include "CardinalEngine/Rendering/RHI/RHIContextImmediate.h"
#include "CardinalEngine/Rendering/ShaderCompiler.h"
#include "CardinalEngine/Rendering/RHI/Shader.h"
#include "CardinalEngine/Rendering/RHI/RHIBuffer.h"
#include "CardinalEngine/Rendering/RHI/RHIImage.h"
#include "CardinalEngine/Material/Texture.h"

#include "CardinalEngine/FileSystem/Paths.h"

namespace CE
{
	Material::Material(const MaterialSettings& Settings, Array<String>* Errors) : m_Settings(Settings)
	{
		ReconstructMaterial(Errors);
	}

	bool Material::IsValid() const
	{
		return m_PBRPipeline != nullptr && m_PBRPipeline->IsValid() && m_DepthPipeline != nullptr && m_DepthPipeline->IsValid();
	}

	bool Material::SetParameter(const String& Name, const char* Data, size_t DataSize, bool UploadToGPU /*= true*/)
	{
		size_t Offset = 0;
		for (auto& Param : m_DynamicParamBufferDesc.Variables)
		{
			if (Param.Name == Name)
			{
				if (Param.Size != DataSize)
				{
					return false;
				}
				memcpy(m_DynamicParamValues.Data() + Offset, Data, DataSize);
				if (UploadToGPU)
				{
					UploadParametersToGPU();
				}
				return true;
			}

			Offset += Param.Size;
		}
		return false;
	}

	bool Material::SetParameter(const String& Name, const bool& Value, bool UploadToGPU /*= true*/)
	{
		const int32 IValue = Value;
		return SetParameter(Name, (const char*)&IValue, sizeof(IValue), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const float& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const Vec2f& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const Vec3f& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const Vec4f& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const Mat4x4f& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const int32& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const Vec2i& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const Vec3i& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const Vec4i& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const uint32& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const Vec2ui& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const Vec3ui& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::SetParameter(const String& Name, const Vec4ui& Value, bool UploadToGPU /*= true*/)
	{
		return SetParameter(Name, (const char*)&Value, sizeof(Value), UploadToGPU);
	}

	bool Material::GetParameter(const String& Name, char* Data, size_t DataSize) const
	{
		size_t Offset = 0;
		for (auto& Param : m_DynamicParamBufferDesc.Variables)
		{
			if (Param.Name == Name)
			{
				if (Param.Size != DataSize)
				{
					return false;
				}
				memcpy(Data, m_DynamicParamValues.Data() + Offset, DataSize);
				return true;
			}

			Offset += Param.Size;
		}
		return false;
	}

	bool Material::GetParameter(const String& Name, bool& Value) const
	{
		int32 IValue = Value;
		if (GetParameter(Name, (char*)&IValue, sizeof(IValue)))
		{
			Value = IValue;
			return true;
		}
		return false;
	}

	bool Material::GetParameter(const String& Name, float& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, Vec2f& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, Vec3f& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, Vec4f& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, Mat4x4f& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, int32& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, Vec2i& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, Vec3i& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, Vec4i& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, uint32& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, Vec2ui& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, Vec3ui& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	bool Material::GetParameter(const String& Name, Vec4ui& Value) const
	{
		return GetParameter(Name, (char*)&Value, sizeof(Value));
	}

	void Material::UploadParametersToGPU()
	{
		Array<char> Values = m_DynamicParamValues;
		SharedPtr<RHIBuffer> Buffer = m_DynamicParamBuffer;
		RenderThread::Enqueue([=](RHIContextImmediate& Context)
			{
				void* Mapped = Context.Map(Buffer, ERHIMapAccess::Write);
		if (Mapped != nullptr)
		{
			memcpy(Mapped, Values.Data(), Values.Size() * sizeof(char));
			Context.UnMap(Buffer);
		}
			});
	}

	String Material::LoadShaderCode(const std::filesystem::path& Path) const
	{
		std::filesystem::path FilePath = Paths::EditorResources();
		FilePath /= "Shaders/";
		FilePath /= Path;
		FilePath = FilePath.lexically_normal();

		std::ifstream File = std::ifstream(FilePath);
		if (File.is_open())
		{
			File.seekg(0, std::ios::end);
			size_t size = File.tellg();
			String Result;
			Result.Append(' ', size);
			File.seekg(0);
			File.read(&Result[0], size);
			File.close();

			return Result;
		}

		return String::Format("#error Failed to load Shader: %s",  FilePath.u8string().c_str());
	}

	bool Material::CreateShaders(RHIPipeline_GraphicShaders& Shaders, bool IsDepth, Array<String>* Errors) const
	{
		String GeneratedShaderCode;
		GeneratedShaderCode += LoadShaderCode("Material/Material_Data.hlsl");
		GeneratedShaderCode += LoadShaderCode("Material/Material_Inputs.hlsl");
		GeneratedShaderCode += LoadShaderCode("Material/Material_Buffers.hlsl");
		GeneratedShaderCode += m_Settings.ShaderSource;
		if (m_Settings.ShaderSource.Find("void CalculateMaterial_Vertex(inout Material OutMaterial, Material_InputVertex Input)") == String::NPos)
		{
			GeneratedShaderCode += "\nvoid CalculateMaterial_Vertex(inout Material OutMaterial, Material_InputVertex Input) {}\n";
		}
		if (!IsDepth && m_Settings.ShaderSource.Find("void CalculateMaterial_Fragment(inout Material OutMaterial, Material_InputFragment Input)") == String::NPos)
		{
			GeneratedShaderCode += "\nvoid CalculateMaterial_Fragment(inout Material OutMaterial, Material_InputFragment Input) {}\n";
		}
		GeneratedShaderCode += LoadShaderCode("Material/Material_Vertex.hlsl");

		if (IsDepth)
		{
			GeneratedShaderCode += LoadShaderCode("Material/Material_Fragment_Depth.hlsl");
		}
		else
		{
			switch (m_Settings.Shading)
			{
			case EMaterialShading::Unlit:
				GeneratedShaderCode += LoadShaderCode("Material/Material_Fragment_Unlit.hlsl");
				break;
			default:
			case EMaterialShading::DefaultLit:
				GeneratedShaderCode += LoadShaderCode("Material/Material_Fragment_DefaultLit.hlsl");
				break;
			}
		}

		ShaderCompiler SCompiler;
		SCompiler.Defines["CE_IS_DEPTH"] = IsDepth ? "1" : "0";
		SCompiler.Defines["CE_BUILD_MATERIAL"] = "1";

		Array<char> CompiledVertexShaderCode;
		if (!SCompiler.CompileToHLSL(EShaderStage::Vertex, EShaderSourceLanguage::HLSL, GeneratedShaderCode.Data(), GeneratedShaderCode.Size(), "VShader", Errors, CompiledVertexShaderCode))
		{
			if (Errors != nullptr)
			{
				Errors->Add("Failed to Compile Vertex Shader");
			}
			else
			{
				CE_LOG(ELog::Error, "Failed to create Material Vertex Shader");
			}
			return false;
		}

		Array<char> CompiledFragmentShaderCode;
		if (!SCompiler.CompileToHLSL(EShaderStage::Fragment, EShaderSourceLanguage::HLSL, GeneratedShaderCode.Data(), GeneratedShaderCode.Size(), "FShader", Errors, CompiledFragmentShaderCode))
		{
			if (Errors != nullptr)
			{
				Errors->Add("Failed to Compile Fragment Shader");
			}
			else
			{
				CE_LOG(ELog::Error, "Failed to create Material Fragment Shader");
			}
			return false;
		}

		Shaders.VertexShader = Engine::Get().DynamicRHI().CreateVertexShader(CompiledVertexShaderCode.Data(), CompiledVertexShaderCode.Size(), "VShader");
		Shaders.TessellationControlShader = nullptr;
		Shaders.TessellationEvaluationShader = nullptr;
		Shaders.GeometryShader = nullptr;
		Shaders.FragmentShader = Engine::Get().DynamicRHI().CreateFragmentShader(CompiledFragmentShaderCode.Data(), CompiledFragmentShaderCode.Size(), "FShader");
	
		return true;
	}

	bool Material::ReflectShaders(const RHIPipeline_GraphicShaders& Shaders, ShaderReflection& Reflection, std::array<int32, (int)EMaterialBufferType::_COUNT>& BufferBindings, ShaderReflection_Buffer& DynamicParameterBuffer, Map<String, SamplerInfo>& Samplers, Map<String, TextureInfo>& Textures, Array<String>* Errors)
	{
		Shaders.VertexShader->Reflect(Reflection, Errors);
		if (Shaders.VertexShader == nullptr)
		{
			if (Errors != nullptr)
			{
				Errors->Add("Failed to create Vertex Shader");
			}
			else
			{
				CE_LOG(ELog::Error, "Failed to create Material Vertex Shader");
			}
			return false;
		}
		if (Shaders.TessellationControlShader != nullptr)
		{
			Shaders.TessellationControlShader->Reflect(Reflection, Errors);
		}
		if (Shaders.TessellationEvaluationShader != nullptr)
		{
			Shaders.TessellationEvaluationShader->Reflect(Reflection, Errors);
		}
		if (Shaders.GeometryShader != nullptr)
		{
			Shaders.GeometryShader->Reflect(Reflection, Errors);
		}
		if (Shaders.FragmentShader == nullptr)
		{
			if (Errors != nullptr)
			{
				Errors->Add("Failed to create Fragment Shader");
			}
			else
			{
				CE_LOG(ELog::Error, "Failed to create Material Fragment Shader");
			}
			return false;
		}
		Shaders.FragmentShader->Reflect(Reflection, Errors);


		auto FindMaterialBufferBinding = [&](const String& Name, const FunctionRef<void(uint32, ShaderReflection_Buffer&)>& Func)
		{
			auto Found = std::find_if(Reflection.Buffers.begin(), Reflection.Buffers.end(), [&](const std::pair<const uint32, ShaderReflection_Buffer>& It) { return It.second.Name == Name; });
			if (Found != Reflection.Buffers.end())
			{
				Func(Found->first, Found->second);
			}
		};



		for (size_t i = 0; i < (int)EMaterialBufferType::_COUNT; i++)
		{
			BufferBindings[i] = -1;
		}

		FindMaterialBufferBinding("Uniform_Frame", [&](int32 Binding, ShaderReflection_Buffer& SRB) { BufferBindings[(int)EMaterialBufferType::Frame] = Binding; });
		FindMaterialBufferBinding("Uniform_Enviroment", [&](int32 Binding, ShaderReflection_Buffer& SRB) { BufferBindings[(int)EMaterialBufferType::Environment] = Binding; });
		FindMaterialBufferBinding("Uniform_Camera", [&](int32 Binding, ShaderReflection_Buffer& SRB) { BufferBindings[(int)EMaterialBufferType::Camera] = Binding; });
		FindMaterialBufferBinding("DynamicParameters", [&](int32 Binding, ShaderReflection_Buffer& SRB)
			{
				BufferBindings[(int)EMaterialBufferType::DynamicParameters] = Binding;
		DynamicParameterBuffer = SRB;
			});


		for (auto& ReflectedSampler : Reflection.Samplers)
		{
			Samplers.emplace(ReflectedSampler.second.Name, SamplerInfo{ ReflectedSampler.first, ReflectedSampler.second });
		}

		for (auto& ReflectedImage : Reflection.Images)
		{
			Textures.emplace(ReflectedImage.second.Name, TextureInfo{ ReflectedImage.first, ReflectedImage.second });
		}

		return true;
	}

	void Material::CreatePipelineDescription(RHIPipeline_GraphicDescription& Description, const std::array<int32, (int)EMaterialBufferType::_COUNT>& BufferBindings, bool IsDepth, Array<String>* Errors) const
	{
		if (IsDepth)
		{
			Description.RenderTargetFormats.Clear();
			Description.DepthStencilState.DepthWriteEnabled = true;
		}
		else
		{
			Description.RenderTargetFormats = { EDataFormat::RGBA8_UNorm };
			Description.DepthStencilState.DepthWriteEnabled = false;
			Description.DepthStencilState.DepthCompare = ECompareOp::Equal;
		}

		Description.DepthStencilFormat = EDataFormat::D32_Float;
		Description.Rasterizer.CullMode = m_Settings.TwoSided ? ECullMode::None : ECullMode::Back;
		Description.Rasterizer.WireFrame = m_Settings.Wireframe;

		switch (m_Settings.Domain)
		{
		default:
		case EMaterialDomain::Geometry:
		{
			Description.VertexInputLayout = MeshVertexLayout;
			Description.VertexInputLayout.Emplace(ShaderVertexInput(Description.VertexInputLayout.Size(), EDataFormat::RGBA32_Float, 0, 1, true));
			Description.VertexInputLayout.Emplace(ShaderVertexInput(Description.VertexInputLayout.Size(), EDataFormat::RGBA32_Float, 0, 1, true));
			Description.VertexInputLayout.Emplace(ShaderVertexInput(Description.VertexInputLayout.Size(), EDataFormat::RGBA32_Float, 0, 1, true));
			Description.VertexInputLayout.Emplace(ShaderVertexInput(Description.VertexInputLayout.Size(), EDataFormat::RGBA32_Float, 0, 1, true));

			if (BufferBindings[(int)EMaterialBufferType::Frame] != -1)
			{
				Description.ResourceBindings.EmplaceRef(ShaderResourceBinding(BufferBindings[(int)EMaterialBufferType::Frame], EShaderResourceType::UniformBuffer));
			}
			if (BufferBindings[(int)EMaterialBufferType::Environment] != -1)
			{
				Description.ResourceBindings.EmplaceRef(ShaderResourceBinding(BufferBindings[(int)EMaterialBufferType::Environment], EShaderResourceType::UniformBuffer));
			}
			if (BufferBindings[(int)EMaterialBufferType::Camera] != -1)
			{
				Description.ResourceBindings.EmplaceRef(ShaderResourceBinding(BufferBindings[(int)EMaterialBufferType::Camera], EShaderResourceType::UniformBuffer));
			}
			if (BufferBindings[(int)EMaterialBufferType::StaticParameters] != -1)
			{
				Description.ResourceBindings.EmplaceRef(ShaderResourceBinding(BufferBindings[(int)EMaterialBufferType::StaticParameters], EShaderResourceType::UniformBuffer));
			}
			if (BufferBindings[(int)EMaterialBufferType::DynamicParameters] != -1)
			{
				Description.ResourceBindings.EmplaceRef(ShaderResourceBinding(BufferBindings[(int)EMaterialBufferType::DynamicParameters], EShaderResourceType::UniformBuffer));
			}
			break;
		}
		}

		switch (m_Settings.Blend)
		{
		default:
		case EMaterialBlend::Opaque:
			// Pipelines Default is Opaque so do nothing here
			break;
		case EMaterialBlend::Translucent:
			RenderTargetBlend& Blend = Description.BlendState.RenderTargetBlends.EmplaceRef();
			Blend.BlendEnabled = true;
			Blend.SrcBlend = EBlendFactor::SrcAlpha;
			Blend.DstBlend = EBlendFactor::InvSrcAlpha;
			Blend.BlendOp = EBlendOperation::Add;
			Blend.SrcBlendAlpha = EBlendFactor::One;
			Blend.DstBlendAlpha = EBlendFactor::InvSrcAlpha;
			Blend.BlendOpAlpha = EBlendOperation::Add;
			Blend.LogicOp = ELogicOperation::Copy;
			Blend.WriteMask = EBlendMask::AllComponents;
			break;
		}
	}

	bool Material::CreateParameterBuffer(SharedPtr<RHIBuffer>& ParameterBuffer, Array<char>& ParameterValues, int32 Size, Array<String>* Errors)
	{
		RHIBufferDescription ParameterBufferDesc;
		ParameterBufferDesc.Size = Size;
		ParameterBufferDesc.ElementSize = ParameterBufferDesc.Size;
		ParameterBufferDesc.Usage = ERHIBufferUsage::UniformBuffer;
		ParameterBufferDesc.Access = ERHIAccess::GPURead_CPUWrite;
		ParameterBuffer = Engine::Get().DynamicRHI().CreateBuffer(ParameterBufferDesc);

		if (!ParameterBuffer->IsValid())
		{
			if (Errors != nullptr)
			{
				Errors->Add("Failed to create Parameter Buffer");
			}
			else
			{
				CE_LOG(ELog::Error, "Failed to create Material Parameter Buffer");
			}
			return false;
		}

		ParameterValues.AddZeroed(ParameterBufferDesc.Size);
		return true;
	}

	void Material::ReconstructMaterial(Array<String>* Errors)
	{
		if (m_Settings.ShaderLanguage != EShaderSourceLanguage::HLSL)
		{
			if (Errors != nullptr)
			{
				Errors->Add("Currently the Material only supports HLSL");
			}
			else
			{
				CE_LOG(ELog::Error, "Failed to create Material: Currently the Material only supports HLSL");
			}
			return;
		}

		RHIPipeline_GraphicShaders PBRShaders;
		if (!CreateShaders(PBRShaders, false, Errors))
		{
			return;
		}

		RHIPipeline_GraphicShaders DepthShaders;
		if (!CreateShaders(DepthShaders, true, Errors))
		{
			return;
		}

		ShaderReflection ShaderReflection;
		std::array<int32, (int)EMaterialBufferType::_COUNT> NewBufferBindings;
		ShaderReflection_Buffer NewDynamicParamBufferDesc;
		Map<String, SamplerInfo> Samplers;
		Map<String, TextureInfo> Textures;
		if (!ReflectShaders(PBRShaders, ShaderReflection, NewBufferBindings, NewDynamicParamBufferDesc, Samplers, Textures, Errors))
		{
			return;
		}

		RHIPipeline_GraphicDescription PBRPipelineDesc;
		CreatePipelineDescription(PBRPipelineDesc, NewBufferBindings, false, Errors);
		
		RHIPipeline_GraphicDescription DepthPipelineDesc;
		CreatePipelineDescription(DepthPipelineDesc, NewBufferBindings, true, Errors);

		SharedPtr<RHIPipeline_Graphic> PBRPipeline = Engine::Get().DynamicRHI().CreatePipelineGraphic(PBRShaders, PBRPipelineDesc);
		if (!PBRPipeline->IsValid())
		{
			if (Errors != nullptr)
			{
				Errors->Add("Failed to create PBR Pipeline");
			}
			else
			{
				CE_LOG(ELog::Error, "Failed to create Material PBR Pipeline");
			}
			return;
		}

		SharedPtr<RHIPipeline_Graphic> DepthPipeline = Engine::Get().DynamicRHI().CreatePipelineGraphic(DepthShaders, DepthPipelineDesc);
		if (!DepthPipeline->IsValid())
		{
			if (Errors != nullptr)
			{
				Errors->Add("Failed to create Depth Pipeline");
			}
			else
			{
				CE_LOG(ELog::Error, "Failed to create Material Depth Pipeline");
			}
			return;
		}

		SharedPtr<RHIBuffer> NewDynamicParamBuffer = nullptr;
		Array<char> NewDynamicParamValues;
		if (NewDynamicParamBufferDesc.Size > 0)
		{
			if (!CreateParameterBuffer(NewDynamicParamBuffer, NewDynamicParamValues, NewDynamicParamBufferDesc.Size, Errors))
			{
				return;
			}
		}

		m_PBRPipeline = PBRPipeline;
		m_DepthPipeline = DepthPipeline;
		m_DynamicParamBufferDesc = NewDynamicParamBufferDesc;
		m_DynamicParamBuffer = NewDynamicParamBuffer;
		m_DynamicParamValues = NewDynamicParamValues;

		m_BufferBindings = NewBufferBindings;
		m_Samplers = Samplers;
		m_Textures = Textures;
	}

	void Material::SerializeAsset(Archive& Ar)
	{
		Ar << m_Settings.Domain;
		Ar << m_Settings.Blend;
		Ar << m_Settings.Shading;
		Ar << m_Settings.ShaderLanguage;
		Ar << m_Settings.ShaderSource;
		Ar << m_Settings.TwoSided;
		Ar << m_Settings.Wireframe;

		if (Ar.IsLoading())
		{
			ReconstructMaterial(nullptr);
		}
	}
}
