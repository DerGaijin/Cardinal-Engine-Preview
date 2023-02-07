#include "CardinalEngine/Rendering/ShaderCompiler.h"
#include "CardinalEngine/Framework/Platform/Platform.h"
//#include "CardinalEngine/Rendering/RHI/DynamicRHI.h"

#define ENABLE_HLSL
#include "glslang/Public/ShaderLang.h"
#include "glslang/SPIRV/GlslangToSpv.h"

#if CE_PLATFORM == CE_PLATFORM_WINDOWS
#include <windows.h>
#include "DirectX11/D3Dcompiler.h"
#endif

namespace CE
{
	void* ShaderCompiler::m_D3DCompilerLibrary = nullptr;
	void* ShaderCompiler::m_D3DCompilePtr = nullptr;

	const TBuiltInResource DefaultTBuiltInResource = {
		/* .MaxLights = */ 32,
		/* .MaxClipPlanes = */ 6,
		/* .MaxTextureUnits = */ 32,
		/* .MaxTextureCoords = */ 32,
		/* .MaxVertexAttribs = */ 64,
		/* .MaxVertexUniformComponents = */ 4096,
		/* .MaxVaryingFloats = */ 64,
		/* .MaxVertexTextureImageUnits = */ 32,
		/* .MaxCombinedTextureImageUnits = */ 80,
		/* .MaxTextureImageUnits = */ 32,
		/* .MaxFragmentUniformComponents = */ 4096,
		/* .MaxDrawBuffers = */ 32,
		/* .MaxVertexUniformVectors = */ 128,
		/* .MaxVaryingVectors = */ 8,
		/* .MaxFragmentUniformVectors = */ 16,
		/* .MaxVertexOutputVectors = */ 16,
		/* .MaxFragmentInputVectors = */ 15,
		/* .MinProgramTexelOffset = */ -8,
		/* .MaxProgramTexelOffset = */ 7,
		/* .MaxClipDistances = */ 8,
		/* .MaxComputeWorkGroupCountX = */ 65535,
		/* .MaxComputeWorkGroupCountY = */ 65535,
		/* .MaxComputeWorkGroupCountZ = */ 65535,
		/* .MaxComputeWorkGroupSizeX = */ 1024,
		/* .MaxComputeWorkGroupSizeY = */ 1024,
		/* .MaxComputeWorkGroupSizeZ = */ 64,
		/* .MaxComputeUniformComponents = */ 1024,
		/* .MaxComputeTextureImageUnits = */ 16,
		/* .MaxComputeImageUniforms = */ 8,
		/* .MaxComputeAtomicCounters = */ 8,
		/* .MaxComputeAtomicCounterBuffers = */ 1,
		/* .MaxVaryingComponents = */ 60,
		/* .MaxVertexOutputComponents = */ 64,
		/* .MaxGeometryInputComponents = */ 64,
		/* .MaxGeometryOutputComponents = */ 128,
		/* .MaxFragmentInputComponents = */ 128,
		/* .MaxImageUnits = */ 8,
		/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
		/* .MaxCombinedShaderOutputResources = */ 8,
		/* .MaxImageSamples = */ 0,
		/* .MaxVertexImageUniforms = */ 0,
		/* .MaxTessControlImageUniforms = */ 0,
		/* .MaxTessEvaluationImageUniforms = */ 0,
		/* .MaxGeometryImageUniforms = */ 0,
		/* .MaxFragmentImageUniforms = */ 8,
		/* .MaxCombinedImageUniforms = */ 8,
		/* .MaxGeometryTextureImageUnits = */ 16,
		/* .MaxGeometryOutputVertices = */ 256,
		/* .MaxGeometryTotalOutputComponents = */ 1024,
		/* .MaxGeometryUniformComponents = */ 1024,
		/* .MaxGeometryVaryingComponents = */ 64,
		/* .MaxTessControlInputComponents = */ 128,
		/* .MaxTessControlOutputComponents = */ 128,
		/* .MaxTessControlTextureImageUnits = */ 16,
		/* .MaxTessControlUniformComponents = */ 1024,
		/* .MaxTessControlTotalOutputComponents = */ 4096,
		/* .MaxTessEvaluationInputComponents = */ 128,
		/* .MaxTessEvaluationOutputComponents = */ 128,
		/* .MaxTessEvaluationTextureImageUnits = */ 16,
		/* .MaxTessEvaluationUniformComponents = */ 1024,
		/* .MaxTessPatchComponents = */ 120,
		/* .MaxPatchVertices = */ 32,
		/* .MaxTessGenLevel = */ 64,
		/* .MaxViewports = */ 16,
		/* .MaxVertexAtomicCounters = */ 0,
		/* .MaxTessControlAtomicCounters = */ 0,
		/* .MaxTessEvaluationAtomicCounters = */ 0,
		/* .MaxGeometryAtomicCounters = */ 0,
		/* .MaxFragmentAtomicCounters = */ 8,
		/* .MaxCombinedAtomicCounters = */ 8,
		/* .MaxAtomicCounterBindings = */ 1,
		/* .MaxVertexAtomicCounterBuffers = */ 0,
		/* .MaxTessControlAtomicCounterBuffers = */ 0,
		/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
		/* .MaxGeometryAtomicCounterBuffers = */ 0,
		/* .MaxFragmentAtomicCounterBuffers = */ 1,
		/* .MaxCombinedAtomicCounterBuffers = */ 1,
		/* .MaxAtomicCounterBufferSize = */ 16384,
		/* .MaxTransformFeedbackBuffers = */ 4,
		/* .MaxTransformFeedbackInterleavedComponents = */ 64,
		/* .MaxCullDistances = */ 8,
		/* .MaxCombinedClipAndCullDistances = */ 8,
		/* .MaxSamples = */ 4,
		/* .maxMeshOutputVerticesNV = */ 256,
		/* .maxMeshOutputPrimitivesNV = */ 512,
		/* .maxMeshWorkGroupSizeX_NV = */ 32,
		/* .maxMeshWorkGroupSizeY_NV = */ 1,
		/* .maxMeshWorkGroupSizeZ_NV = */ 1,
		/* .maxTaskWorkGroupSizeX_NV = */ 32,
		/* .maxTaskWorkGroupSizeY_NV = */ 1,
		/* .maxTaskWorkGroupSizeZ_NV = */ 1,
		/* .maxMeshViewCountNV = */ 4,
		/* .maxDualSourceDrawBuffersEXT = */ 1,

		/* .limits = */ {
			/* .nonInductiveForLoops = */ 1,
			/* .whileLoops = */ 1,
			/* .doWhileLoops = */ 1,
			/* .generalUniformIndexing = */ 1,
			/* .generalAttributeMatrixVectorIndexing = */ 1,
			/* .generalVaryingIndexing = */ 1,
			/* .generalSamplerIndexing = */ 1,
			/* .generalVariableIndexing = */ 1,
			/* .generalConstantMatrixVectorIndexing = */ 1,
		}
	};

	bool ShaderCompiler::Initialize()
	{
#if CE_PLATFORM == CE_PLATFORM_WINDOWS
		m_D3DCompilerLibrary = Platform::Get().LoadDynamicLibrary("D3DCompiler_47");
		if (m_D3DCompilerLibrary != nullptr)
		{
			m_D3DCompilePtr = Platform::Get().GetFunctionPointer(m_D3DCompilerLibrary, "D3DCompile");
		}
#endif

		if (!glslang::InitializeProcess())
		{
			return false;
		}

		return true;
	}

	void ShaderCompiler::Terminate()
	{
		glslang::FinalizeProcess();

		m_D3DCompilePtr = nullptr;

		Platform::Get().UnloadDynamicLibrary(m_D3DCompilerLibrary);
	}

	bool ShaderCompiler::TranslateHLSLtoSPIRV(EShaderStage Stage, const char* Data, size_t DataSize, const char* Entry, Array<char>& CompileResult, Array<String>* CompileErrors)
	{
		EShLanguage Language = EShLanguage::EShLangVertex;
		switch (Stage)
		{
		default:
		case CE::EShaderStage::Vertex:
			Language = EShLanguage::EShLangVertex;
			break;
		case CE::EShaderStage::TessellationControl:
			Language = EShLanguage::EShLangTessControl;
			break;
		case CE::EShaderStage::TessellationEvaluation:
			Language = EShLanguage::EShLangTessEvaluation;
			break;
		case CE::EShaderStage::Geometry:
			Language = EShLanguage::EShLangGeometry;
			break;
		case CE::EShaderStage::Fragment:
			Language = EShLanguage::EShLangFragment;
			break;
		case CE::EShaderStage::Compute:
			Language = EShLanguage::EShLangCompute;
			break;
		}

		glslang::TShader ShaderCompiler = glslang::TShader(Language);
		ShaderCompiler.setEntryPoint(Entry);
		ShaderCompiler.setSourceEntryPoint(Entry);
		ShaderCompiler.setEnvInput(glslang::EShSourceHlsl, Language, glslang::EShClientVulkan, 100);

		ShaderCompiler.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
		ShaderCompiler.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);
		ShaderCompiler.setStringsWithLengths(&Data, (int*)&DataSize, 1);
		ShaderCompiler.setAutoMapBindings(true);

		// HLSL Specific
		ShaderCompiler.setFlattenUniformArrays(true);
		ShaderCompiler.setHlslIoMapping(true);
		//Compiler.setEnvTargetHlslFunctionality1();

		TBuiltInResource Resources = DefaultTBuiltInResource;
		EShMessages Messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules | EShMsgReadHlsl | EShMsgHlslLegalization);
		if (!ShaderCompiler.parse(&Resources, 100, false, Messages))
		{
			if (CompileErrors != nullptr)
			{
				CompileErrors->EmplaceRef(ShaderCompiler.getInfoLog());
			}
			else
			{
				CE_LOG(ELog::Warning, "Failed to Compile Shader from HLSL to SPIRV: %s", ShaderCompiler.getInfoLog());
			}
			return false;
		}

		glslang::TProgram Program;
		Program.addShader(&ShaderCompiler);

		if (!Program.link(Messages))
		{
			if (CompileErrors != nullptr)
			{
				CompileErrors->EmplaceRef(Program.getInfoLog());
			}
			else
			{
				CE_LOG(ELog::Warning, "Failed to Compile Shader from HLSL to SPIRV: %s", Program.getInfoLog());
			}
			return false;
		}

		// Will Auto map binding the Buffer/Samplers/Textures
		Program.mapIO();

		glslang::SpvOptions Options;
		Options.disableOptimizer = false;
		Options.optimizeSize = true;

		CE_ASSERT(false, "HLSLToSPIRV does not support Includes and Defines");

		std::vector<unsigned int> CompiledSpirv;
		glslang::GlslangToSpv(*Program.getIntermediate(Language), CompiledSpirv, &Options);
		CompileResult.Clear(CompiledSpirv.size() * sizeof(uint32));
		CompileResult.AddUninitialized(CompiledSpirv.size() * sizeof(uint32));
		memcpy(CompileResult.Data(), CompiledSpirv.data(), CompiledSpirv.size() * sizeof(uint32));

		return true;
	}

	bool ShaderCompiler::TranslateGLSLtoSPIRV(EShaderStage Stage, const char* Data, size_t DataSize, const char* Entry, Array<char>& CompileResult, Array<String>* CompileErrors)
	{
		CE_LOG(ELog::Warning, "Shader Compile GLSL to SPIRV is unimplemented!");
		return false;
	}

	bool ShaderCompiler::TranslateSPIRVtoHLSL(EShaderStage Stage, const char* Data, size_t DataSize, const char* Entry, Array<char>& CompileResult, Array<String>* CompileErrors)
	{
		CE_LOG(ELog::Warning, "Shader Compile SPIRV to HLSL is unimplemented!");
		return false;
	}

	bool ShaderCompiler::CompileToHLSL(EShaderStage Stage, EShaderSourceLanguage SourceLanguage, const char* Source, size_t SourceSize, const char* Entry, Array<String>* CompileErrors, Array<char>& Output)
	{
#if CE_PLATFORM != CE_PLATFORM_WINDOWS
		CE_LOG(ELog::Error, "Failed to Compile Shader To HLSL on this Platform, please use Windows to compile HLSL Shaders");
		return false;
#endif

		Array<char> HLSLSourceStorage;
		const char* HLSLSource = nullptr;
		size_t HLSLSourceSize = 0;

		switch (SourceLanguage)
		{
		case CE::EShaderSourceLanguage::HLSL:
		{
			HLSLSource = Source;
			HLSLSourceSize = SourceSize;
			break;
		}
		case CE::EShaderSourceLanguage::GLSL:
		{
			Array<char> SPIRV;
			if (!TranslateGLSLtoSPIRV(Stage, Source, SourceSize, Entry, SPIRV, CompileErrors))
			{
				return false;
			}

			if (!TranslateSPIRVtoHLSL(Stage, SPIRV.Data(), SPIRV.Size(), Entry, HLSLSourceStorage, CompileErrors))
			{
				return false;
			}

			HLSLSource = HLSLSourceStorage.Data();
			HLSLSourceSize = HLSLSourceStorage.Size();
			break;
		}
		default:
		{
			if (CompileErrors != nullptr)
			{
				CompileErrors->Emplace("The SourceLanguage is unknown");
			}
			else
			{
				CE_LOG(ELog::Error, "Failed to Compile Shader To HLSL because the SourceLanguage is unknown");
			}
			return false;
		}
		}

#if CE_PLATFORM == CE_PLATFORM_WINDOWS
		char* ShaderVer = nullptr;
		switch (Stage)
		{
		default:
		case EShaderStage::Vertex:
			ShaderVer = "vs_5_0";
			break;
		case EShaderStage::TessellationControl:
			ShaderVer = "hs_5_0";
			break;
		case EShaderStage::TessellationEvaluation:
			ShaderVer = "ds_5_0";
			break;
		case EShaderStage::Geometry:
			ShaderVer = "gs_5_0";
			break;
		case EShaderStage::Fragment:
			ShaderVer = "ps_5_0";
			break;
		case EShaderStage::Compute:
			ShaderVer = "cs_5_0";
			break;
		}

		using D3DCompile_t = HRESULT(*)(LPCVOID pSrcData, SIZE_T srcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs);
		D3DCompile_t CompileFn = (D3DCompile_t)m_D3DCompilePtr;

		CE_CHECK(CompileFn != nullptr);

		Array<D3D_SHADER_MACRO> Macros;
		Macros.Reserve(Defines.size() + 1);
		for (auto& Def : Defines)
		{
			D3D_SHADER_MACRO& Macro = Macros.EmplaceRef();

			Macro.Name = Def.first.Data();
			Macro.Definition = Def.second.Data();
		}
		Macros.Add({ NULL, NULL });

		struct Includer : public ID3DInclude
		{
			Includer(ShaderCompiler& Compiler) : Compiler(Compiler) {}
			ShaderCompiler& Compiler;

			virtual HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
			{
				CE_CHECK(DataPtr == nullptr);

				auto Found = Compiler.Includes.find(pFileName);
				if (Found != Compiler.Includes.end())
				{
					DataPtr = Found->second.Data();
					ppData = &DataPtr;
					return S_OK;
				}
				return S_FALSE;
			}
			
			virtual HRESULT Close(LPCVOID pData)
			{
				DataPtr = nullptr;
				return S_OK;
			}

			const void* DataPtr = nullptr;
		} Incl(*this);

		ID3DBlob* ShaderByteCode = nullptr;
		ID3DBlob* ErrorBlob = nullptr;
		CompileFn(HLSLSource, HLSLSourceSize, 0, Macros.Data(), &Incl, Entry, ShaderVer, 0, 0, &ShaderByteCode, &ErrorBlob);
		if (ErrorBlob != nullptr)
		{
			std::string CompileError((const char*)ErrorBlob->GetBufferPointer(), ErrorBlob->GetBufferSize() - 2);
			if (CompileErrors != nullptr)
			{
				CompileErrors->Emplace(CompileError);
			}
			else
			{
				CE_LOG(ELog::Warning, "Failed to Create Shader: %s", CompileError.c_str());
			}
			ErrorBlob->Release();
			ErrorBlob = nullptr;
			return false;
		}

		Output.Clear(ShaderByteCode->GetBufferSize());
		Output.AddUninitialized(ShaderByteCode->GetBufferSize());
		memcpy(Output.Data(), ShaderByteCode->GetBufferPointer(), ShaderByteCode->GetBufferSize());
		ShaderByteCode->Release();
		ShaderByteCode = nullptr;
#endif
		return true;
	}

	bool ShaderCompiler::CompileToSPIRV(EShaderStage Stage, EShaderSourceLanguage SourceLanguage, const char* Source, size_t SourceSize, const char* Entry, Array<String>* CompileErrors, Array<char>& Output)
	{
		switch (SourceLanguage)
		{
		case CE::EShaderSourceLanguage::HLSL:
		{
			if (!TranslateHLSLtoSPIRV(Stage, Source, SourceSize, Entry, Output, CompileErrors))
			{
				return false;
			}
			break;
		}
		case CE::EShaderSourceLanguage::GLSL:
		{
			if (!TranslateGLSLtoSPIRV(Stage, Source, SourceSize, Entry, Output, CompileErrors))
			{
				return false;
			}
			break;
		}
		default:
		{
			if (CompileErrors != nullptr)
			{
				CompileErrors->Emplace("The SourceLanguage is unknown");
			}
			else
			{
				CE_LOG(ELog::Error, "Failed to Compile Shader To SPIRV because the SourceLanguage is unknown");
			}
			return false;
		}
		}

		return true;
	}
}
