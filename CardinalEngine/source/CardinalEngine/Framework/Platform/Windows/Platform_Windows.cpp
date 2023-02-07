#include "Platform_Windows.h"

#include <windows.h>
#include <Shlobj.h>
#undef max

namespace CE
{
	bool Platform_Windows::ShowOpenFileDialog(Array<std::filesystem::path>& OutFilePaths, const Text& Title, const Array<std::pair<Text, Text>>& Filter, EFileDialogFlag Flags /*= EFileDialogFlag::None*/)
	{
		wchar_t FileNames[1024];

		size_t FilterSize = 0;
		for (auto& Filter : Filter)
		{
			FilterSize += Filter.first.Size() + 1;
			FilterSize += Filter.second.Size() + 1;
		}
		FilterSize++;

		wchar_t* FilterStr = new wchar_t[FilterSize];
		size_t It = 0;
		for (auto& Filter : Filter)
		{
			BasicStringFunctions<wchar_t>::Copy(FilterStr + It, Filter.first.Data(), Filter.first.Size() + 1);
			It += Filter.first.Size() + 1;

			BasicStringFunctions<wchar_t>::Copy(FilterStr + It, Filter.second.Data(), Filter.second.Size() + 1);
			It += Filter.second.Size() + 1;
		}
		BasicStringFunctions<wchar_t>::Set(FilterStr + It, 1, TEXT('\0'));

		OPENFILENAME ofn;
		memset(&FileNames, 0, sizeof(FileNames));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = FilterStr;
		ofn.lpstrFile = FileNames;
		ofn.nMaxFile = 1024;
		ofn.lpstrTitle = Title.Data();

		if (Flags & EFileDialogFlag::ReadOnly)
		{
			ofn.Flags |= OFN_READONLY;
		}
		if (Flags & EFileDialogFlag::HideReadOnly)
		{
			ofn.Flags |= OFN_HIDEREADONLY;
		}
		if (Flags & EFileDialogFlag::AllowMulti)
		{
			ofn.Flags |= OFN_ALLOWMULTISELECT | OFN_EXPLORER;
		}
		if (Flags & EFileDialogFlag::PathMustExist)
		{
			ofn.Flags |= OFN_PATHMUSTEXIST;
		}
		if (Flags & EFileDialogFlag::FileMustExist)
		{
			ofn.Flags |= OFN_FILEMUSTEXIST;
		}
		if (Flags & EFileDialogFlag::DontAddToRecent)
		{
			ofn.Flags |= OFN_DONTADDTORECENT;
		}
		if (Flags & EFileDialogFlag::ForceShowHidden)
		{
			ofn.Flags |= OFN_FORCESHOWHIDDEN;
		}

		bool Result = ::GetOpenFileNameW(&ofn);

		delete[] FilterStr;

		if (Result)
		{
			if (Flags & EFileDialogFlag::AllowMulti)
			{
				std::filesystem::path Directory;

				wchar_t* Ptr = ofn.lpstrFile;
				Ptr[ofn.nFileOffset - 1] = 0;
				Directory = Ptr;
				Ptr += ofn.nFileOffset;

				while (*Ptr)
				{
					OutFilePaths.Add(Directory / std::filesystem::path(Ptr));
					Ptr += (Text::CharFunctions::Length(Ptr) + 1);
				}
			}
			else
			{
				OutFilePaths.Add(FileNames);
			}
		}

		return Result;
	}

	bool Platform_Windows::ShowSaveFileDialog(std::filesystem::path& SavePath, const Text& Title, EFileDialogFlag Flags /*= EFileDialogFlag::None*/)
	{
		wchar_t FileName[MAX_PATH];

		OPENFILENAME ofn;
		memset(&FileName, 0, sizeof(FileName));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = NULL;
		ofn.lpstrFile = FileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = Title.Data();

		if (Flags & EFileDialogFlag::ReadOnly)
		{
			ofn.Flags |= OFN_READONLY;
		}
		if (Flags & EFileDialogFlag::HideReadOnly)
		{
			ofn.Flags |= OFN_HIDEREADONLY;
		}
		if (Flags & EFileDialogFlag::PathMustExist)
		{
			ofn.Flags |= OFN_PATHMUSTEXIST;
		}
		if (Flags & EFileDialogFlag::FileMustExist)
		{
			ofn.Flags |= OFN_FILEMUSTEXIST;
		}
		if (Flags & EFileDialogFlag::DontAddToRecent)
		{
			ofn.Flags |= OFN_DONTADDTORECENT;
		}
		if (Flags & EFileDialogFlag::ForceShowHidden)
		{
			ofn.Flags |= OFN_FORCESHOWHIDDEN;
		}

		bool Result = ::GetSaveFileNameW(&ofn);

		if (Result)
		{
			SavePath = FileName;
		}

		return Result;
	}

	bool Platform_Windows::ShowBrowseFolderDialog(std::filesystem::path& FolderPath, const Text& Title)
	{
		bool Result = false;
		IFileOpenDialog* dialog;
		if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void**)&dialog)))
		{
			DWORD options;
			dialog->GetOptions(&options);
			dialog->SetOptions(options | FOS_PICKFOLDERS);
			if (SUCCEEDED(dialog->Show(NULL)))
			{
				IShellItem* selectedItem;
				if (SUCCEEDED(dialog->GetResult(&selectedItem)))
				{
					PWSTR pszFilePath;
					if (SUCCEEDED(selectedItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszFilePath)))
					{
						FolderPath = pszFilePath;
						Result = true;
						CoTaskMemFree(pszFilePath);
					}

					selectedItem->Release();
				}
			}

			dialog->Release();
		}

		return Result;
	}

	void* Platform_Windows::LoadDynamicLibrary(const char* FileName)
	{
		return ::LoadLibraryA(FileName);
	}

	void* Platform_Windows::LoadDynamicLibrary(const wchar_t* FileName)
	{
		return ::LoadLibraryW(FileName);
	}

	void* Platform_Windows::GetLibraryFromAddress(void* Address)
	{
		HMODULE hModule = NULL;
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)Address, &hModule);
		return hModule;
	}

	void* Platform_Windows::GetFunctionPointer(void* Library, const char* FunctionName)
	{
		return ::GetProcAddress((HMODULE)Library, FunctionName);
	}

	std::filesystem::path Platform_Windows::GetLibraryPath(void* Library)
	{
		char path[MAX_PATH];
		GetModuleFileNameA((HMODULE)Library, path, sizeof(path));
		return std::filesystem::path(path).lexically_normal();
	}

	bool Platform_Windows::UnloadDynamicLibrary(void* Library)
	{
		return ::FreeLibrary((HMODULE)Library);
	}

	bool Platform_Windows::StartProcess(const std::filesystem::path& PathToProcess, const Text& StartArguments /*= Text()*/)
	{
		// additional information
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// start the program up

		Text FinalArguments = PathToProcess.c_str();
		FinalArguments.Append(TEXT(" "));
		FinalArguments.Append(StartArguments);
		wchar_t* Args = const_cast<wchar_t*>(FinalArguments.Data());
		if (!CreateProcessW(NULL,   // the path
			Args,			// Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		))
		{
			return false;
		}

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}

	void Platform_Windows::GenerateGUID(Guid& Guid) const
	{
		CE_CHECK(::CoCreateGuid((::GUID*)&Guid) == S_OK);
	}

#if CE_DEBUG
	bool Platform_Windows::SetCurrentThreadDebugName(const Text& Name)
	{
		return ::SetThreadDescription(::GetCurrentThread(), Name.Data()) == 0;
	}
#endif
}
