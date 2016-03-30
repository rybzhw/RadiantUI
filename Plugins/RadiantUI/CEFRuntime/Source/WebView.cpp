// Copyright 2014 Joseph Riedel. All Rights Reserved.

#include "Variants.hpp"
#include "WebView.hpp"

ICefRuntimeVariantFactory* WebView::GetVariantFactory()
{
	return GetStaticVariantFactory();
}

void WebView::Bind(Handler* InClient)
{
	Client = InClient;
}

void WebView::Resize(int InSizeX, int InSizeY)
{
	if ((InSizeX > 0) && (InSizeY > 0))
	{
		Client->Resize(InSizeX, InSizeY);
	}
}

void WebView::LoadURL(const char *InURL)
{
	Client->LoadURL(InURL);
}

void WebView::SetMouseCursorChangeDisabled(bool InDisabled)
{
	Client->GetHost()->SetMouseCursorChangeDisabled(InDisabled);
}

void WebView::ExecuteJSHook(const char* InHookName, ICefRuntimeVariantList* InArguments)
{
	CefRefPtr<CefProcessMessage> Message = CefProcessMessage::Create(InHookName);
	if (InArguments)
	{
		SetCefListFromVariantList(Message->GetArgumentList(), InArguments);
	}
	Client->GetBrowser()->SendProcessMessage(PID_RENDERER, Message);
}

bool WebView::IsMouseCursorChangeDisabled()
{
	return Client->GetHost()->IsMouseCursorChangeDisabled();
}

void WebView::WasHidden(bool InHidden)
{
	Client->GetHost()->WasHidden(InHidden);
}

void WebView::SendKeyEvent(const CefRuntimeKeyEvent& InEvent)
{
	Client->GetHost()->SendKeyEvent(Convert(InEvent));
}

void WebView::SendMouseClickEvent(const CefRuntimeMouseEvent& InEvent,
	ECefRuntimeMouseButton InMouseButton,
	bool bInMouseUp, int InClickCount)
{
	Client->GetHost()->SendMouseClickEvent(Convert(InEvent), Convert(InMouseButton), bInMouseUp, InClickCount);
}

void WebView::SendMouseMoveEvent(const CefRuntimeMouseEvent& InEvent,
	bool bInMouseLeave)
{
	Client->GetHost()->SendMouseMoveEvent(Convert(InEvent), bInMouseLeave);
}

void WebView::SendMouseWheelEvent(const CefRuntimeMouseEvent& InEvent,
	int InDeltaX, int InDeltaY)
{
	Client->GetHost()->SendMouseWheelEvent(Convert(InEvent), InDeltaX, InDeltaY);
}

void WebView::SendFocusEvent(bool InSetFocus)
{
	Client->GetHost()->SendFocusEvent(InSetFocus);
}

void WebView::SendCaptureLostEvent()
{
	Client->GetHost()->SendCaptureLostEvent();
}

void* WebView::GetNSTextInputContext()
{
	return Client->GetHost()->GetNSTextInputContext();
}

void WebView::HandleKeyEventBeforeTextInputClient(void* InKeyEvent)
{
	Client->GetHost()->HandleKeyEventBeforeTextInputClient((CefEventHandle)InKeyEvent);
}

void WebView::HandleKeyEventAfterTextInputClient(void* InKeyEvent)
{
	Client->GetHost()->HandleKeyEventAfterTextInputClient((CefEventHandle)InKeyEvent);
}

bool WebView::CanGoBack()
{
	return Client->GetBrowser()->CanGoBack();
}

void WebView::GoBack()
{
	Client->GetBrowser()->GoBack();
}

bool WebView::CanGoForward()
{
	return Client->GetBrowser()->CanGoForward();
}

void WebView::GoForward()
{
	Client->GetBrowser()->GoForward();
}

bool WebView::IsLoading()
{
	return Client->GetBrowser()->IsLoading();
}

void WebView::Reload()
{
	Client->GetBrowser()->Reload();
}

void WebView::ReloadIgnoreCache()
{
	Client->GetBrowser()->ReloadIgnoreCache();
}

void WebView::StopLoad()
{
	Client->GetBrowser()->StopLoad();
}

void WebView::Release()
{
	Client->CloseExistingBrowser();
	delete this;
}

CefKeyEvent WebView::Convert(const CefRuntimeKeyEvent& InEvent)
{
	CefKeyEvent Event;
	Event.character = InEvent.Character;
	Event.focus_on_editable_field = InEvent.FocusOnEditableField;
	Event.is_system_key = InEvent.IsSystemKey;
	Event.modifiers = InEvent.Modifiers;
	Event.native_key_code = InEvent.NativeKeyCode;
	Event.type = Convert(InEvent.Type);
	Event.unmodified_character = InEvent.UnmodifiedCharacter;
	Event.windows_key_code = InEvent.WindowsKeyCode;
	return Event;
}

CefMouseEvent WebView::Convert(const CefRuntimeMouseEvent& InEvent)
{
	CefMouseEvent Event;
	Event.modifiers = InEvent.Modifiers;
	Event.x = InEvent.X;
	Event.y = InEvent.Y;
	return Event;
}

cef_mouse_button_type_t WebView::Convert(ECefRuntimeMouseButton InButtons)
{
	return (cef_mouse_button_type_t)InButtons;
}

cef_key_event_type_t WebView::Convert(ECefRuntimeKeyEvent InKeyState)
{
	return (cef_key_event_type_t)InKeyState;
}
