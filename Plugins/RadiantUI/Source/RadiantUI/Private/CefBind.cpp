// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"
#include "CefBind.h"
#include "AllowWindowsPlatformTypes.h"
#include <windows.h>
#include <string>
#include "HideWindowsPlatformTypes.h"

namespace
{
#if defined(WIN32)
#pragma warning (disable:4191)
#if defined(RADIANTUI_DEBUG) // from RadiantUI.Build.cs
#define DLLNAME "CEFFramework-Debug.dll"
#define MODULENAME "UE4Editor-RadiantUI-Win64-DebugGame.dll"
#else
#define DLLNAME "CEFFramework.dll"
#define MODULENAME "UE4Editor-RadiantUI.dll"
#endif

/*
#if !WITH_EDITOR
#undef MODULENAME
#define MODULENAME NULL
#endif
*/

//#define FRAMEWORKPATH "..\\..\\CefRuntime\\Binaries\\Release\\"DLLNAME
#define FRAMEWORKPATH "Plugins\\RadiantUI\\CefRuntime\\Binaries\\Release\\"DLLNAME

	static ICefRuntimeAPI* LoadCefFrameworkDLL(ICefRuntimeCallbacks *InCallbacks)
	{
		UE_LOG(RadiantUILog, Log, TEXT("Loading CEF Framework"));

		static bool bLoaded = false;
		if (bLoaded)
		{
			return nullptr;
		}

		bLoaded = true;

		/*
		HMODULE Module = GetModuleHandleA(MODULENAME);
		if (Module == NULL)
		{
			UE_LOG(RadiantUILog, Error, TEXT("Unable to get module handle"));
			return nullptr;
		}

		char szModuleName[256];

		if (0 == GetModuleFileNameA(Module, szModuleName, 255))
		{
			UE_LOG(RadiantUILog, Error, TEXT("Unable to get module name"));
			return nullptr;
		}

		szModuleName[255] = 0;

		char szModulePath[256];
		char *szFilePart;

		if (0 == GetFullPathNameA(szModuleName, 255, szModulePath, &szFilePart))
		{
			UE_LOG(RadiantUILog, Error, TEXT("Unable to get path for %s"), FRAMEWORKPATH);
			return nullptr;
		}

		if (szFilePart)
		{
			*szFilePart = 0;
		}

		strcat_s(szModulePath, 256, FRAMEWORKPATH);
		*/

		FString RelativePath = FPaths::ProjectDir();
		FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
		FullPath += TEXT(FRAMEWORKPATH);
		UE_LOG(RadiantUILog, Log, TEXT("Loading CEF Framework: %s"), *FullPath);
		// UE_LOG(RadiantUILog, Log, TEXT("Loading CEF Framework: %s"), *szModulePath);

		// SetDllDirectory(NULL);
		// AddDllDirectory(L"A:\\Unreal Projects\\ShooterGame\\Plugins\\RadiantUI\\CefRuntime\\Binaries\\Win64");
		// HMODULE Library = LoadLibraryExA(szModulePath, NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
		// HMODULE Library = LoadLibraryExA(, NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
		// "A:\\Unreal Projects\\ShooterGame\\Plugins\\RadiantUI\\CefRuntime\\Binaries\\Release\\CEFFramework.dll"
		HMODULE Library = LoadLibraryExA(TCHAR_TO_ANSI(*FullPath), NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
		
		if (Library == NULL)
		{
			UE_LOG(RadiantUILog, Error, TEXT("Unable to load CefRuntime from %s"), *FullPath);
			return nullptr;
		}

		CreateCefRuntimeAPI f = (CreateCefRuntimeAPI)GetProcAddress(Library, CEFCREATERUNTIMEAPI_SIG);
		return f ? f(InCallbacks) : NULL;
	}
#endif

}

ICefRuntimeAPI* CefStartup(ICefRuntimeCallbacks* InCallbacks)
{
	return LoadCefFrameworkDLL(InCallbacks);
}
