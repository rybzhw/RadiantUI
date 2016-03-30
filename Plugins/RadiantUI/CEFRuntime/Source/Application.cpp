// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "Application.hpp"
#include "Variants.hpp"
#include "Assert.hpp"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"


class JSHookExtensionHandler : public CefV8Handler
{
	IMPLEMENT_REFCOUNTING(JSHookExtensionHandler);
public:
	explicit JSHookExtensionHandler(CefRefPtr<Application> InApp) : App(InApp) {}

	virtual bool Execute(const CefString& InName,
		CefRefPtr<CefV8Value> InObject,
		const CefV8ValueList& InArguments,
		CefRefPtr<CefV8Value>& OutRetval,
		CefString& OutException)
	{
		bool Handled = false;
		bool ValidAPI = true;

		if (InName == "TriggerEvent")
		{
			// dispatch hook to the browser process for execution on the game thread
			if ((InArguments.size() == 1) && InArguments[0]->IsArray())
			{
				CefRefPtr<CefV8Value> ArgumentArray = InArguments[0];
				if (ArgumentArray->GetArrayLength() > 0)
				{
					CefRefPtr<CefBrowser> Browser = CefV8Context::GetCurrentContext()->GetBrowser();
					ASSERT(Browser.get());

					CefString HookName = ArgumentArray->GetValue(0)->GetStringValue();
					if (!HookName.empty())
					{
						CefRefPtr<CefProcessMessage> Message = CefProcessMessage::Create(HookName);

						// translate remaining args.
						if ((ArgumentArray->GetArrayLength() > 1) && ArgumentArray->GetValue(1)->IsArray())
						{
							CefRefPtr<CefV8Value> InParameters = ArgumentArray->GetValue(1);
							const int NumParameters = InParameters->GetArrayLength();

							CefRefPtr<CefListValue> OutParameters = Message->GetArgumentList();
							OutParameters->SetSize(NumParameters);

							for (int i = 0; i < (int)NumParameters; ++i)
							{
								V8ValueToListItem_RenderThread(InParameters->GetValue(i), OutParameters, i);
							}
						}

						Browser->SendProcessMessage(PID_BROWSER, Message);
						Handled = true;
					}
				}
			}
		}
		else if (InName == "SetHook")
		{
			if ((InArguments.size() == 2) && (InArguments[0]->IsString()) && (InArguments[1]->IsFunction()))
			{
				CefString HookName = InArguments[0]->GetStringValue().ToString();
				CefRefPtr<CefV8Context> Context = CefV8Context::GetCurrentContext();
				App->SetJSHook(HookName, Context->GetBrowser()->GetIdentifier(), Context, InArguments[1]);
				Handled = true;
			}
		}
		else if (InName == "RemoveHook")
		{
			if ((InArguments.size() == 1) && InArguments[0]->IsString())
			{
				CefRefPtr<CefV8Context> Context = CefV8Context::GetCurrentContext();
				App->RemoveJSHook(InArguments[0]->GetStringValue(), Context->GetBrowser()->GetIdentifier());
				Handled = true;
			}
		}
		else
		{
			ValidAPI = false;
			OutException = std::string("Unrecognized JSHook API Call: '") + InName.ToString() + std::string("'");
		}

		if (!Handled && ValidAPI)
		{
			OutException = std::string("Invalid Arguments Passed To '") + InName.ToString() + std::string("'");
		}

		return Handled;
	}
private:

	CefRefPtr<Application> App;
};

void Application::SetJSHook(
	const CefString& InHookName,
	int InBrowserID,
	CefRefPtr<CefV8Context> InContext,
	CefRefPtr<CefV8Value> InFunction
	)
{
	REQUIRE_RENDER_THREAD()

	JSHook Hook;
	Hook.Context = InContext;
	Hook.Function = InFunction;

	Hooks.insert(JSHookMap::value_type(std::make_pair(InHookName.ToString(), InBrowserID), Hook));
}

void Application::RemoveJSHook(const CefString& InHookName, int InBrowserId)
{
	REQUIRE_RENDER_THREAD()

		Hooks.erase(std::make_pair(InHookName.ToString(), InBrowserId));
}

// CefRenderProcessHandler methods.
void Application::OnWebKitInitialized()
{
	// Register our hook extension
	std::string script =
		"var RadiantUI;"
		"if (!RadiantUI)"
		"  RadiantUI = {};"
		"(function() {"
		"  RadiantUI.TriggerEvent = function() {"
		"    native function TriggerEvent();"
		"    return TriggerEvent(Array.prototype.slice.call(arguments));"
		"  };"
		"  RadiantUI.SetCallback = function(name, callback) {"
		"    native function SetHook();"
		"    return SetHook(name, callback);"
		"  };"
		"  RadiantUI.RemoveCallback = function(name) {"
		"    native function RemoveHook();"
		"    return RemoveHook(name);"
		"  };"
		"})();";
	CefRegisterExtension("RadiantUI JSHooks Extension", script, new JSHookExtensionHandler(this));
}

void Application::OnContextReleased(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context)
{
	for (JSHookMap::iterator it = Hooks.begin(); it != Hooks.end();)
	{
		if (it->second.Context->IsSame(context))
		{
			JSHookMap::iterator next = it; ++next;
			Hooks.erase(it);
			it = next;
		}
		else
		{
			++it;
		}
	}
}

void Application::OnFocusedNodeChanged(
	CefRefPtr<CefBrowser> InBrowser,
	CefRefPtr<CefFrame> InFrame,
	CefRefPtr<CefDOMNode> InNode)
{
	bool IsEditableNode = InNode.get() && InNode->IsEditable();

	CefRefPtr<CefProcessMessage> Message = CefProcessMessage::Create(RADUIIPCMSG_FOCUSNODECHANGED);
	Message->GetArgumentList()->SetBool(0, IsEditableNode);
	InBrowser->SendProcessMessage(PID_BROWSER, Message);
}

bool Application::OnProcessMessageReceived(
	CefRefPtr<CefBrowser> InBrowser,
	CefProcessId InSourceProcess,
	CefRefPtr<CefProcessMessage> InMessage)
{
	ASSERT(InSourceProcess == PID_BROWSER); // call should have come from browser process.

	if (!Hooks.empty())
	{
		CefString MessageName = InMessage->GetName();
		JSHookMap::iterator it = Hooks.find(std::make_pair(MessageName, InBrowser->GetIdentifier()));
		if (it != Hooks.end())
		{
			// invoke JS callback
			JSHook Hook(it->second);

			Hook.Context->Enter();

			CefRefPtr<CefListValue> MessageArguments = InMessage->GetArgumentList();
			const int NumMessageArguments = (int)MessageArguments->GetSize();

			// convert message arguments
			CefV8ValueList Arguments;

			for (int i = 0; i < NumMessageArguments; ++i)
			{
				Arguments.push_back(ListItemToV8Value_RenderThread(MessageArguments, i));
			}

			Hook.Function->ExecuteFunction(nullptr, Arguments);
			Hook.Context->Exit();
			return true;
		}
	}

	return false;
}

void Application::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line)
{
	/*
	command_line->AppendSwitch("allow-no-sandbox-job");
	command_line->AppendSwitch("disable-gpu-sandbox");
	command_line->AppendSwitch("disable-3d-apis");
	command_line->AppendSwitch("disable-webgl");
	*/

	command_line->AppendSwitch("off-screen-rendering-enabled");
	command_line->AppendSwitchWithValue("off-screen-frame-rate", "60");
	command_line->AppendSwitch("enable-font-antialiasing");
	command_line->AppendSwitch("enable-media-stream");

	command_line->AppendSwitch("disable-gpu");
	command_line->AppendSwitch("disable-gpu-compositing");
	command_line->AppendSwitch("enable-begin-frame-scheduling");
};


/*
void Application::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();

  // Information used when creating the native window.
  CefWindowInfo window_info;

#if defined(OS_WIN)
  // On Windows we need to specify certain flags that will be passed to
  // CreateWindowEx().
  window_info.SetAsPopup(NULL, "cefsimple");
#endif

  // SimpleHandler implements browser-level callbacks.
  CefRefPtr<SimpleHandler> handler(new SimpleHandler());

  // Specify CEF browser settings here.
  CefBrowserSettings browser_settings;

  std::string url;

  // Check if a "--url=" value was provided via the command-line. If so, use
  // that instead of the default URL.
  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
  url = command_line->GetSwitchValue("url");
  if (url.empty())
    url = "http://www.google.com";

  // Create the first browser window.
  CefBrowserHost::CreateBrowser(window_info, handler.get(), url,
                                browser_settings, NULL);
}
*/