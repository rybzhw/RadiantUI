// Copyright 2014 Joseph Riedel. All Rights Reserved.

#pragma once

#include "include/cef_app.h"

#include "../API/CEFRuntimeAPI.hpp"
#include "Handler.hpp"

class WebView : public ICefWebView
{
public:

	void Bind(Handler* InClient);

	virtual ICefRuntimeVariantFactory* GetVariantFactory() OVERRIDE;

	virtual void Resize(int InSizeX, int InSizeY);
	virtual void LoadURL(const char *InURL);

	virtual void ExecuteJSHook(const char* InHookName, ICefRuntimeVariantList* InArguments);

	///
	// Set whether mouse cursor change is disabled.
	///
	/*--cef()--*/
	virtual void SetMouseCursorChangeDisabled(bool InDisabled);

	///
	// Returns true if mouse cursor change is disabled.
	///
	/*--cef()--*/
	virtual bool IsMouseCursorChangeDisabled();

	///
	// Notify the browser that it has been hidden or shown. Layouting and
	// CefRenderHandler::OnPaint notification will stop when the browser is
	// hidden. This method is only used when window rendering is disabled.
	///
	/*--cef()--*/
	virtual void WasHidden(bool InHidden);

	///
	// Send a key event to the browser.
	///
	/*--cef()--*/
	virtual void SendKeyEvent(const CefRuntimeKeyEvent& InEvent);

	///
	// Send a mouse click event to the browser. The |x| and |y| coordinates are
	// relative to the upper-left corner of the view.
	///
	/*--cef()--*/
	virtual void SendMouseClickEvent(const CefRuntimeMouseEvent& InEvent,
		ECefRuntimeMouseButton InMouseButton,
		bool bInMouseUp, int InClickCount);

	///
	// Send a mouse move event to the browser. The |x| and |y| coordinates are
	// relative to the upper-left corner of the view.
	///
	/*--cef()--*/
	virtual void SendMouseMoveEvent(const CefRuntimeMouseEvent& InEvent,
		bool bInMouseLeave);

	///
	// Send a mouse wheel event to the browser. The |x| and |y| coordinates are
	// relative to the upper-left corner of the view. The |deltaX| and |deltaY|
	// values represent the movement delta in the X and Y directions respectively.
	// In order to scroll inside select popups with window rendering disabled
	// CefRenderHandler::GetScreenPoint should be implemented properly.
	///
	/*--cef()--*/
	virtual void SendMouseWheelEvent(const CefRuntimeMouseEvent& InEvent,
		int InDeltaX, int InDeltaY);

	///
	// Send a focus event to the browser.
	///
	/*--cef()--*/
	virtual void SendFocusEvent(bool InSetFocus);

	///
	// Send a capture lost event to the browser.
	///
	/*--cef()--*/
	virtual void SendCaptureLostEvent();

	///
	// Get the NSTextInputContext implementation for enabling IME on Mac when
	// window rendering is disabled.
	///
	/*--cef(default_retval=NULL)--*/
	virtual void* GetNSTextInputContext();

	///
	// Handles a keyDown event prior to passing it through the NSTextInputClient
	// machinery.
	///
	/*--cef()--*/
	virtual void HandleKeyEventBeforeTextInputClient(void* InKeyEvent);

	///
	// Performs any additional actions after NSTextInputClient handles the event.
	///
	/*--cef()--*/
	virtual void HandleKeyEventAfterTextInputClient(void* InKeyEvent);

	///
	// Returns true if the browser can navigate backwards.
	///
	/*--cef()--*/
	virtual bool CanGoBack();

	///
	// Navigate backwards.
	///
	/*--cef()--*/
	virtual void GoBack();

	///
	// Returns true if the browser can navigate forwards.
	///
	/*--cef()--*/
	virtual bool CanGoForward();

	///
	// Navigate forwards.
	///
	/*--cef()--*/
	virtual void GoForward();

	///
	// Returns true if the browser is currently loading.
	///
	/*--cef()--*/
	virtual bool IsLoading();

	///
	// Reload the current page.
	///
	/*--cef()--*/
	virtual void Reload();

	///
	// Reload the current page ignoring any cached data.
	///
	/*--cef()--*/
	virtual void ReloadIgnoreCache();

	///
	// Stop loading the page.
	///
	/*--cef()--*/
	virtual void StopLoad();

	virtual void Release();

private:

	CefKeyEvent Convert(const CefRuntimeKeyEvent& InEvent);
	CefMouseEvent Convert(const CefRuntimeMouseEvent& InEvent);
	cef_mouse_button_type_t Convert(ECefRuntimeMouseButton InButtons);
	cef_key_event_type_t Convert(ECefRuntimeKeyEvent InKeyState);

	CefRefPtr<Handler> Client;
};