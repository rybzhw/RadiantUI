// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
// Copyright 2014 Joseph Riedel. All Rights Reserved.

#pragma once

#include "include/cef_client.h"
#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/base/cef_lock.h"
#include "include/cef_request_handler.h"

#include "../API/CEFRuntimeAPI.hpp"

#include <set>
#include <string>

class Handler : public CefClient,
	public CefContextMenuHandler,
	public CefDisplayHandler,
	public CefDownloadHandler,
	public CefDragHandler,
	public CefGeolocationHandler,
	public CefKeyboardHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefRenderHandler,
	public CefRequestHandler
{
	private:

		IMPLEMENT_REFCOUNTING(Handler);

		base::Lock lock_;

		bool ParseURL(const std::string& URL, std::string& OutPath, std::string& OutMimeType);

		int SizeX;
		int SizeY;
		bool InEditableField;
		ICefWebView* WebView;
		ICefWebViewCallbacks *Callbacks;
		CefRefPtr<CefBrowser> Browser;

	public:

		Handler(int InSizeX, int InSizeY, ICefWebView* InWebView, ICefWebViewCallbacks *InCallbacks);
		virtual ~Handler();

		void Resize(int InSizeX, int InSizeY);

		void CloseExistingBrowser();
		void LoadURL(const CefString& InURL);

		CefRefPtr<CefBrowser> GetBrowser() { return Browser; }
		CefRefPtr<CefBrowserHost> GetHost() { return Browser->GetHost(); }

		virtual CefRefPtr<CefRenderHandler> GetRenderHandler()
		{
			return this;
		}

		// CefClient methods
		virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE
		{
			return this;
		}

		virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE
		{
			return this;
		}

		virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE
		{
			return this;
		}

		virtual CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE
		{
			return this;
		}

		virtual CefRefPtr<CefGeolocationHandler> GetGeolocationHandler() OVERRIDE
		{
			return this;
		}

		virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE
		{
			return this;
		}

		virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE
		{
			return this;
		}

		virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE
		{
			return this;
		}

		virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE
		{
			return this;
		}

		virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;

		// CefContextMenuHandler methods
		virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) OVERRIDE;
		virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) OVERRIDE;

		// CefDisplayHandler methods
		virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) OVERRIDE;
		virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
		virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line) OVERRIDE;

		// CefDownloadHandler methods
		virtual void OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;
		virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, CefRefPtr<CefDownloadItemCallback> callback) OVERRIDE;

		// CefDragHandler methods
		virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask) OVERRIDE;

		// CefGeolocationHandler methods
		virtual bool OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser, const CefString& requesting_url, int request_id, CefRefPtr<CefGeolocationCallback> callback) OVERRIDE;

		// CefKeyboardHandler methods
		virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut) OVERRIDE;
		
		virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			const CefString& target_url,
			const CefString& target_frame_name,
			CefLifeSpanHandler::WindowOpenDisposition target_disposition,
			bool user_gesture,
			const CefPopupFeatures& popupFeatures,
			CefWindowInfo& windowInfo,
			CefRefPtr<CefClient>& client,
			CefBrowserSettings& settings,
			bool* no_javascript_access
			) OVERRIDE;

		virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
		virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

		// CefLoadHandler methods
		virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
			bool isLoading,
			bool canGoBack,
			bool canGoForward
			) OVERRIDE;

		virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			ErrorCode errorCode,
			const CefString& errorText,
			const CefString& failedUrl
			) OVERRIDE;

		// CefRequestHandler methods
		virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request
			) OVERRIDE;

		virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
			const CefString& origin_url,
			int64 new_size,
			CefRefPtr<CefRequestCallback> callback
			) OVERRIDE;

		virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
			const CefString& url,
			bool& allow_os_execution
			) OVERRIDE;

		virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
			TerminationStatus status
			) OVERRIDE;

		// CefRenderHandler methods
		virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
		virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
		virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY) OVERRIDE;
		virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) OVERRIDE;
		virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;
		virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) OVERRIDE;

		virtual void OnCursorChange(CefRefPtr<CefBrowser> browser,
			CefCursorHandle cursor,
			CursorType type,
			const CefCursorInfo& custom_cursor_info) OVERRIDE;

		virtual void OnPaint(
			CefRefPtr<CefBrowser> browser,
			PaintElementType type,
			const RectList& dirtyRects,
			const void* buffer,
			int width, int height
			) OVERRIDE;
};

/*
class SimpleHandler : public CefClient,
                      public CefDisplayHandler,
                      public CefLifeSpanHandler,
                      public CefLoadHandler {
 public:
  SimpleHandler();
  ~SimpleHandler();

  // Provide access to the single global instance of this object.
  static SimpleHandler* GetInstance();

  // CefClient methods:
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
    return this;
  }

  // CefDisplayHandler methods:
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title) OVERRIDE;

  // CefLifeSpanHandler methods:
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

  // CefLoadHandler methods:
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) OVERRIDE;

  // Request that all existing browser windows close.
  void CloseAllBrowsers(bool force_close);

  bool IsClosing() const { return is_closing_; }

 private:
  // List of existing browser windows. Only accessed on the CEF UI thread.
  typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
  BrowserList browser_list_;

  bool is_closing_;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(SimpleHandler);
};
*/
