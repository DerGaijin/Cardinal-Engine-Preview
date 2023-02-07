#pragma once
#include "../Framework/Core/Headers.h"
#include "RHI/Enums.h"

namespace CE
{
	class CE_API ShaderCompiler
	{
	public:
		static bool Initialize();

		static void Terminate();

		bool TranslateHLSLtoSPIRV(EShaderStage Stage, const char* Data, size_t DataSize, const char* Entry, Array<char>& CompileResult, Array<String>* CompileErrors);

		bool TranslateGLSLtoSPIRV(EShaderStage Stage, const char* Data, size_t DataSize, const char* Entry, Array<char>& CompileResult, Array<String>* CompileErrors);

		bool TranslateSPIRVtoHLSL(EShaderStage Stage, const char* Data, size_t DataSize, const char* Entry, Array<char>& CompileResult, Array<String>* CompileErrors);

		bool CompileToHLSL(EShaderStage Stage, EShaderSourceLanguage SourceLanguage, const char* Source, size_t SourceSize, const char* Entry, Array<String>* CompileErrors, Array<char>& Output);

		bool CompileToSPIRV(EShaderStage Stage, EShaderSourceLanguage SourceLanguage, const char* Source, size_t SourceSize, const char* Entry, Array<String>* CompileErrors, Array<char>& Output);

	public:
		Map<String, String> Defines;
		Map<String, String> Includes;

	private:
		static void* m_D3DCompilerLibrary;
		static void* m_D3DCompilePtr;
	};
}
