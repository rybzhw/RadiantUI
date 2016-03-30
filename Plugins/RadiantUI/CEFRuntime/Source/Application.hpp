// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once

#include "include/cef_app.h"

#define RADUIIPCMSG_FOCUSNODECHANGED "RADUIIPC.EditModeChanged"

// Implement application-level callbacks for the browser process.
class Application : public CefApp,
                  public CefBrowserProcessHandler,
				  public CefRenderProcessHandler
{
	private:
		IMPLEMENT_REFCOUNTING(Application);

		struct JSHook
		{
			CefRefPtr<CefV8Context> Context;
			CefRefPtr<CefV8Value> Function;
		};

		typedef std::map<std::pair<std::string, int>, JSHook> JSHookMap;

		JSHookMap Hooks;

	public:

		void SetJSHook(
			const CefString& InHookName,
			int InBrowserID,
			CefRefPtr<CefV8Context> InContext,
			CefRefPtr<CefV8Value> InFunction
			);

		void RemoveJSHook(const CefString& InHookName, int InBrowserId);

		// CefApp
		virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE { return this; }
		virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE { return this; }

		// CefRenderProcessHandler methods.
		virtual void OnWebKitInitialized() OVERRIDE;

		virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) OVERRIDE;

		virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefDOMNode> node) OVERRIDE;

		virtual bool OnProcessMessageReceived(
			CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) OVERRIDE;

		virtual void OnBeforeCommandLineProcessing(
			const CefString& process_type,
			CefRefPtr<CefCommandLine> command_line) OVERRIDE;

};