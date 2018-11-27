#include "EditorIO.h"
#include <Engine/Utilities.h>
#include <Windows.h>
#include <ShObjIdl.h>
#include <string.h> //for wcscat

inline Buffer<COMDLG_FILTERSPEC> ConvertFilters(const Buffer<Pair<const wchar_t*>> &filters)
{
	Buffer<COMDLG_FILTERSPEC> fileTypes;
	fileTypes.SetSize(filters.GetSize());

	for (uint32 i = 0; i < filters.GetSize(); ++i)
	{
		fileTypes[i].pszName = filters[i].first;
		fileTypes[i].pszSpec = filters[i].second;
	}

	return fileTypes;
}

String EditorIO::OpenFileDialog(const wchar_t *dir, const Buffer<Pair<const wchar_t*>> &filters)
{
	Buffer<COMDLG_FILTERSPEC> _fileTypes = ConvertFilters(filters);
	IFileOpenDialog *fileOpenDialog;
	String value = "";

	HRESULT result = ::CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void**)&fileOpenDialog);
	if (SUCCEEDED(result))
	{
		fileOpenDialog->SetFileTypes(_fileTypes.GetSize(), _fileTypes.Data());

		IShellItem *folder;

		WCHAR fullPath[MAX_PATH];
		GetCurrentDirectoryW(MAX_PATH, fullPath);
		wcscat_s(fullPath, dir);
		SHCreateItemFromParsingName(fullPath, NULL, IID_PPV_ARGS(&folder));
		fileOpenDialog->SetFolder(folder);

		result = fileOpenDialog->Show(NULL);
		if (SUCCEEDED(result))
		{
			IShellItem *item;

			result = fileOpenDialog->GetResult(&item);
			if (SUCCEEDED(result))
			{
				LPWSTR chosenPath;
				result = item->GetDisplayName(SIGDN_FILESYSPATH, &chosenPath);

				if (SUCCEEDED(result))
					value = String::FromWideString(chosenPath);

				item->Release();
			}
		}

		fileOpenDialog->Release();
	}

	return value;
}

String EditorIO::SaveFileDialog(const wchar_t *dir, const Buffer<Pair<const wchar_t*>>& filters)
{
	Buffer<COMDLG_FILTERSPEC> _fileTypes = ConvertFilters(filters);
	IFileSaveDialog *fileSaveDialog;
	String value = "";

	HRESULT result = ::CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, (void**)&fileSaveDialog);
	if (SUCCEEDED(result))
	{
		IShellItem *folder;

		WCHAR fullPath[MAX_PATH];
		GetCurrentDirectoryW(MAX_PATH, fullPath);
		wcscat_s(fullPath, dir);
		SHCreateItemFromParsingName(fullPath, NULL, IID_PPV_ARGS(&folder));

		fileSaveDialog->SetDefaultExtension(L"lvl");
		fileSaveDialog->SetFileTypes(_fileTypes.GetSize(), _fileTypes.Data());
		fileSaveDialog->SetFolder(folder);

		result = fileSaveDialog->Show(NULL);
		if (SUCCEEDED(result))
		{
			IShellItem *item;

			result = fileSaveDialog->GetResult(&item);
			if (SUCCEEDED(result))
			{
				LPWSTR chosenPath;
				result = item->GetDisplayName(SIGDN_FILESYSPATH, &chosenPath);

				if (SUCCEEDED(result))
				{
					value = String::FromWideString(chosenPath);
				}

				item->Release();
			}
		}

		fileSaveDialog->Release();
	}

	return value;
}
