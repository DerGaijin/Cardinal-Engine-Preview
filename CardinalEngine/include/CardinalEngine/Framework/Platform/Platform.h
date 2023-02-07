#pragma once
#include "../Core/Headers.h"

namespace CE
{
	class Guid;

	enum class EFileDialogFlag
	{
		None = 0,
		ReadOnly = 1 << 0,
		HideReadOnly = 1 << 1,
		AllowMulti = 1 << 2,
		PathMustExist = 1 << 3,
		FileMustExist = 1 << 4,
		DontAddToRecent = 1 << 5,
		ForceShowHidden = 1 << 6,
	};

	CE_ENUM_BIT_OPERATORS(EFileDialogFlag);

	class CE_API Platform : public Singleton
	{
	public:
		static Platform& Get();

	public:
		// Shows a Modal File dialog to open one or many Files
		virtual bool ShowOpenFileDialog(Array<std::filesystem::path>& OutFilePaths, const Text& Title, const Array<std::pair<Text, Text>>& Filter, EFileDialogFlag Flags = EFileDialogFlag::None) = 0;

		// Shows a Modal File Dialog to save a File
		virtual bool ShowSaveFileDialog(std::filesystem::path& SavePath, const Text& Title, EFileDialogFlag Flags = EFileDialogFlag::None) = 0;

		// Shows a Modal Folder Dialog to select a Folder
		virtual bool ShowBrowseFolderDialog(std::filesystem::path& FolderPath, const Text& Title) = 0;

		// Loads a Dynamic Library into the Engine
		virtual void* LoadDynamicLibrary(const char* FileName) = 0;

		// Loads a Dynamic Library into the Engine
		virtual void* LoadDynamicLibrary(const wchar_t* FileName) = 0;

		// Returns the Dynamic Library Pointer from a Address
		virtual void* GetLibraryFromAddress(void* Address) = 0;

		// Returns a pointer to a function by a FunctionName
		virtual void* GetFunctionPointer(void* Library, const char* FunctionName) = 0;

		// Returns the File Path from the Dynamic Library
		virtual std::filesystem::path GetLibraryPath(void* Library) = 0;

		// Unloads a Dynamic Library
		virtual bool UnloadDynamicLibrary(void* Library) = 0;

		// Starts another Process
		virtual bool StartProcess(const std::filesystem::path& PathToProcess, const Text& StartArguments = Text()) = 0;

		// Generates a Guid
		virtual void GenerateGUID(Guid& Guid) const = 0;

#if CE_DEBUG
		// Set the Name of the currently executing Thread
		virtual bool SetCurrentThreadDebugName(const Text& Name) = 0;
#endif
	};
}
