#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Framework/Platform/Platform.h"

namespace CE
{
	class Platform_Windows : public Platform
	{
	public:
		virtual bool ShowOpenFileDialog(Array<std::filesystem::path>& OutFilePaths, const Text& Title, const Array<std::pair<Text, Text>>& Filter, EFileDialogFlag Flags = EFileDialogFlag::None) override;
		virtual bool ShowSaveFileDialog(std::filesystem::path& SavePath, const Text& Title, EFileDialogFlag Flags = EFileDialogFlag::None) override;
		virtual bool ShowBrowseFolderDialog(std::filesystem::path& FolderPath, const Text& Title) override;
		virtual void* LoadDynamicLibrary(const char* FileName) override;
		virtual void* LoadDynamicLibrary(const wchar_t* FileName) override;
		virtual void* GetLibraryFromAddress(void* Address) override;
		virtual void* GetFunctionPointer(void* Library, const char* FunctionName) override;
		virtual std::filesystem::path GetLibraryPath(void* Library) override;
		virtual bool UnloadDynamicLibrary(void* Library) override;
		virtual bool StartProcess(const std::filesystem::path& PathToProcess, const Text& StartArguments = Text()) override;
		virtual void GenerateGUID(Guid& Guid) const override;

#if CE_DEBUG
		virtual bool SetCurrentThreadDebugName(const Text& Name) override;
#endif
	};
}
