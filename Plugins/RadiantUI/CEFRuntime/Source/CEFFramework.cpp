// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
// Copyright 2014 Joseph Riedel. All Rights Reserved.

#include <windows.h>

#include "Application.hpp"
#include "Variants.hpp"
#include "WebView.hpp"

#include "include/cef_sandbox_win.h"

#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif


inline void DebugString(const char *Text)
{
#if defined(_WIN32)
	OutputDebugStringA(Text);
#endif
}


#if defined(_WIN32)
#pragma warning (disable : 4996)
#if defined(_DEBUG)
#define FRAMEWORKNAME  "CefFramework-Debug.dll"
#define CEFPROCESSNAME "CefProcess-Debug.exe"
#else
#define FRAMEWORKNAME "CefFramework.dll"
#define CEFPROCESSNAME "CefProcess.exe"
#endif
#else
#error "Port Me"
#endif

class CefRuntimeAPI : public ICefRuntimeAPI
{
public:

	CefRefPtr<Application> App;

	virtual ICefRuntimeVariantFactory* GetVariantFactory() OVERRIDE
	{
		return GetStaticVariantFactory();
	}

	bool Initialize(ICefRuntimeCallbacks* InCallbacks)
	{
		Callbacks = InCallbacks;
		App = new Application();

#if defined(_WIN32)
		DebugString("Initializing " FRAMEWORKNAME "...\n");
		HMODULE Module = GetModuleHandleA(FRAMEWORKNAME);
		if (Module == NULL)
		{
			DebugString("GetModuleHandle failed!");
			return false;
		}

		char szModulePath[256];
		if (0 == GetModuleFileNameA(Module, szModulePath, 255))
		{
			DebugString("GetModuleFileName failed!");
			return false;
		}

		szModulePath[255] = 0;

		DebugString("ModulePath: ");
		DebugString(szModulePath);
		DebugString("\n");

		char szSubprocessPath[256];
		char* szFilePart;

		if (0 == GetFullPathNameA(szModulePath, 255, szSubprocessPath, &szFilePart))
		{
			DebugString("GetFullPathNameA failed!");
			return false;
		}

		if (szFilePart)
		{
			*szFilePart = 0;
		}

		szSubprocessPath[255] = 0;

		strcat(szSubprocessPath, CEFPROCESSNAME);

		CefSettings Settings;
		Settings.persist_session_cookies = false;
		Settings.command_line_args_disabled = true;
		Settings.multi_threaded_message_loop = true;
		Settings.single_process = false;

		/*
		CefString(&Settings.locales_dir_path).FromASCII("C:\\Users\\LeGone\\Documents\\Unreal Projects\\Hope7\\Game\\Plugins\\RadiantUI\\ThirdParty\\CEF\\Bin\\locales");
		CefString(&Settings.cache_path).FromASCII("c:\\Cache");
		CefString(&Settings.log_file).FromASCII("c:\\Cache\\debug.log");
		Settings.log_severity = LOGSEVERITY_VERBOSE;
		
		std::string DebugInfo("CEF-CachePath: ");
		DebugInfo += CefString(Settings.cache_path.str);
		DebugInfo += '\n';
		DebugString(DebugInfo.c_str());
		*/
		CefString(&Settings.browser_subprocess_path).FromASCII(szSubprocessPath);

		void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
		// Manage the life span of the sandbox information object. This is necessary
		// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
		CefScopedSandboxInfo scoped_sandbox;
		sandbox_info = scoped_sandbox.sandbox_info();
#endif

		CefMainArgs Args((HINSTANCE)GetModuleHandle(NULL));
		if (!CefInitialize(Args, Settings, App.get(), sandbox_info)) {
			App = NULL;
			return false;
		}

		return true;
#endif
	}

	virtual void CreateWebView(const char* InStartupURL, int InSizeX, int InSizeY, bool InTransparentPainting, ICefWebViewCallbacks *InCallbacks)
	{
		if ((InSizeX < 1) || (InSizeY < 1) || (InStartupURL == nullptr) || (InCallbacks == nullptr))
		{
			return;
		}

		WebView* View = new WebView();

		CefRefPtr<Handler> Client(new Handler(InSizeX, InSizeY, View, InCallbacks));
		View->Bind(Client.get());

		CefWindowInfo WindowInfo;
		CefBrowserSettings BrowserSettings;

		WindowInfo.SetAsWindowless(nullptr, InTransparentPainting);
		WindowInfo.x = 0;
		WindowInfo.y = 0;
		WindowInfo.width = InSizeX;
		WindowInfo.height = InSizeY;

		BrowserSettings.javascript_access_clipboard = STATE_DISABLED;
		BrowserSettings.javascript_close_windows = STATE_DISABLED;
		BrowserSettings.javascript_open_windows = STATE_DISABLED;

		//BrowserSettings.local_storage = STATE_ENABLED;

		CefBrowserHost::CreateBrowser(WindowInfo, Client.get(), CefString(InStartupURL), BrowserSettings, nullptr);
	}

	//! Call this when you are done with the runtime API.
	virtual void Release()
	{
		Callbacks->Release();
		App = NULL;
		//CefShutdown();
		delete this;
	}

	ICefRuntimeCallbacks *Callbacks;
};

#if defined(WIN32)
	#define DLL_API  __declspec(dllexport)
#else
	#define DLL_API
#endif

extern "C" DLL_API ICefRuntimeAPI* CreateCefRuntimeAPI(ICefRuntimeCallbacks* InCallbacks)
{
	CefRuntimeAPI *API = new CefRuntimeAPI();
	if (!API->Initialize(InCallbacks))
	{
		delete API;
		API = nullptr;
	}
	return API;
}