// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
// Copyright 2014 Joseph Riedel. All Rights Reserved.

#include "Assert.hpp"
#include "Handler.hpp"
#include "Application.hpp"
#include "include/cef_parser.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "Variants.hpp"
#include <sstream>
#include <algorithm>

class CefStreamWrapper : public CefReadHandler
{
	IMPLEMENT_REFCOUNTING(CefStreamWrapper);
public:

	CefStreamWrapper(ICefStream* InStream) : Stream(InStream) {}

	virtual ~CefStreamWrapper()
	{
		if (Stream)
		{
			Stream->Release();
		}
	}

	///
	// Read raw binary data.
	///
	/*--cef()--*/
	virtual size_t Read(void* ptr, size_t size, size_t n) OVERRIDE
	{
		return Stream->Read(ptr, (int)(size*n));
	}

	///
	// Seek to the specified offset position. |whence| may be any one of
	// SEEK_CUR, SEEK_END or SEEK_SET. Return zero on success and non-zero on
	// failure.
	///
	/*--cef()--*/
	virtual int Seek(int64 offset, int whence)
	{
		return Stream->Seek((int)offset, (ICefStream::SeekType)whence) ? 1 : 0;
	}

	///
	// Return the current offset position.
	///
	/*--cef()--*/
	virtual int64 Tell()
	{
		return (int64)Stream->GetPos();
	}

	///
	// Return non-zero if at end of file.
	///
	/*--cef()--*/
	virtual int Eof()
	{
		return Stream->IsEOF() ? 1 : 0;
	}

	virtual bool MayBlock()
	{
		return false;
	}

private:

	ICefStream* Stream;
};

Handler::Handler(int InSizeX, int InSizeY, ICefWebView* InWebView, ICefWebViewCallbacks *InCallbacks) : SizeX(InSizeX), SizeY(InSizeY), WebView(InWebView), Callbacks(InCallbacks), InEditableField(false)
{
}

Handler::~Handler()
{
}

void Handler::Resize(int InSizeX, int InSizeY)
{
	if ((SizeX == InSizeX) && (SizeY == InSizeY))
	{
		return;
	}

	base::AutoLock lock_scope(lock_);

	SizeX = InSizeX;
	SizeY = InSizeY;

	if (Browser.get())
	{
		Browser->GetHost()->WasResized();
	}
}

void Handler::CloseExistingBrowser()
{
	if (Browser.get())
	{
		Browser->GetHost()->CloseBrowser(true);
	}
}

void Handler::LoadURL(const CefString& InURL)
{
	if (Browser.get())
	{
		Browser->GetMainFrame()->LoadURL(InURL);
	}
}

bool Handler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	ASSERT(source_process == PID_RENDERER);

	if (message->GetName() == RADUIIPCMSG_FOCUSNODECHANGED)
	{
		InEditableField = message->GetArgumentList()->GetBool(0);
		Callbacks->FocusedNodeChanged(InEditableField);
	}
	else
	{
		ICefRuntimeVariantList* Arguments = CefListToVariant(message->GetArgumentList());
		Callbacks->ExecuteJSHook(message->GetName().ToString().c_str(), Arguments);
		Arguments->Release();
	}

	return true;
}

// CefContextMenuHandler methods
void Handler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	model->Clear(); // suppress context menus
}

bool Handler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags)
{
	return false; // call default implementation
}

// CefDisplayHandler methods
void Handler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
}

void Handler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
}

bool Handler::OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line)
{
	return false;
}

// CefDownloadHandler methods
void Handler::OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback)
{
}

void Handler::OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, CefRefPtr<CefDownloadItemCallback> callback)
{
}

// CefDragHandler methods
bool Handler::OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask)
{
	return false;
}

// CefGeolocationHandler methods
bool Handler::OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser, const CefString& requesting_url, int request_id, CefRefPtr<CefGeolocationCallback> callback)
{
	return false;
}

// CefKeyboardHandler methods
bool Handler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut)
{
	return false;
}

// CefLifeSpanHandler methods
bool Handler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
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
	)
{
	return true;// popups not supported (true == cancel)
}

void Handler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	Browser = browser;
	Callbacks->WebViewCreated(WebView);
}

bool Handler::DoClose(CefRefPtr<CefBrowser> browser)
{
	if ((Browser.get() != NULL) && (Browser->GetIdentifier() == browser->GetIdentifier()))
	{
		browser->GetHost()->CloseBrowser(true); // ParentWindowWillClose()
	}

	return false;
}

void Handler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	if ((Browser.get() != NULL) && (Browser->GetIdentifier() == browser->GetIdentifier()))
	{
		if (Callbacks)
		{
			Callbacks->Release(WebView);
			WebView = nullptr;
		}

		Browser = NULL;
	}
}

// CefLoadHandler methods
void Handler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
	bool isLoading,
	bool canGoBack,
	bool canGoForward
	)
{
}

void Handler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl
	)
{
	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Don't display an error for external protocols that we allow the OS to
	// handle. See OnProtocolExecution().
	/*if (errorCode == ERR_UNKNOWN_URL_SCHEME) {
	std::string urlStr = frame->GetURL();
	if (urlStr.find("spotify:") == 0)
	return;
	}*/

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(failedUrl) <<
		" with error " << std::string(errorText) << " (" << errorCode <<
		").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}

bool Handler::ParseURL(const std::string& URL, std::string& OutPath, std::string& OutMimeType)
{
	/*if ((URL.find(".com") == std::string::npos)
	&& (URL.find(".net") == std::string::npos)
	&& (URL.find(".org") == std::string::npos)
	&& (URL.find("://") == std::string::npos)
	&& (URL.find("www.") == std::string::npos))
	{

	}*/

	CefURLParts Parts;
	CefParseURL(URL, Parts);

	if ((Parts.host.length > 0) && (CefString(&Parts.host) == "content") && (Parts.path.length > 1))
	{
		// local file
		OutPath = CefString(&Parts.path);
		if (OutPath[0] == '/')
		{ // trim leading /
			OutPath = OutPath.substr(1);
		}

		// Figure out mime type.
		size_t pos = OutPath.rfind(".");

		OutMimeType = "text/plain";

		if (pos != std::string::npos)
		{
			std::string Ext = OutPath.substr(pos + 1);

			std::transform(Ext.begin(), Ext.end(), Ext.begin(), ::tolower);

			if (Ext == "html")
			{
				OutMimeType = "text/html";
			}
			else if (Ext == "png")
			{
				OutMimeType = "image/png";
			}
			else if (Ext == "jpg")
			{
				OutMimeType = "image/jpg";
			}
			else if (Ext == "bmp")
			{
				OutMimeType = "image/bmp";
			}
			else if (Ext == "svg")
			{
				OutMimeType = "image/svg+xml";
			}
			else if (Ext == "css")
			{
				OutMimeType = "text/css";
			}
			else if (Ext == "js")
			{
				OutMimeType = "text/javascript";
			}
		}

		return true;
	}

	return false;
}

// CefRequestHandler methods
CefRefPtr<CefResourceHandler> Handler::GetResourceHandler(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request
	)
{
	std::string Path, MimeType;

	if (ParseURL(request->GetURL(), Path, MimeType))
	{
		ICefStream* CefStream = Callbacks->GetFileStream(Path.c_str());
		if (CefStream)
		{
			CefRefPtr<CefReadHandler> ReadHandler(new CefStreamWrapper(CefStream));
			CefRefPtr<CefStreamReader> StreamReader(CefStreamReader::CreateForHandler(ReadHandler));
			return new CefStreamResourceHandler(MimeType, StreamReader);
		}
	}
	return NULL;
}

bool Handler::OnQuotaRequest(CefRefPtr<CefBrowser> browser, const CefString& origin_url, int64 new_size, CefRefPtr<CefRequestCallback> callback)
{
	// don't allow any disk storage access.
	callback->Continue(false);
	return true;
}

void Handler::OnProtocolExecution(CefRefPtr<CefBrowser> browser, const CefString& url, bool& allow_os_execution)
{
	allow_os_execution = true;
}

void Handler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
{
}

// CefRenderHandler methods
bool Handler::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
	base::AutoLock lock_scope(lock_);
	rect = CefRect(0, 0, SizeX, SizeY);
	return true;
}

bool Handler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
	return GetRootScreenRect(browser, rect);
}

bool Handler::GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY)
{
	screenX = viewX;
	screenY = viewY;
	return true;
}

bool Handler::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
	return false;
}

void Handler::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
}

void Handler::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
}

void Handler::OnCursorChange(CefRefPtr<CefBrowser> browser,
	CefCursorHandle cursor,
	CursorType type,
	const CefCursorInfo& custom_cursor_info)
{
	Callbacks->OnCursorChange(cursor);
}

void Handler::OnPaint(
	CefRefPtr<CefBrowser> browser,
	PaintElementType type,
	const RectList& dirtyRects,
	const void* buffer,
	int width, int height
	)
{
	if (!Callbacks->EnterCriticalSection())
	{
		return;
	}

	base::AutoLock lock_scope(lock_);

	if ((type != PET_VIEW) || (SizeX != width) || (SizeY != height))
	{
		Callbacks->LeaveCriticalSection();
		return;
	}

	const int MaxRegions = 64;
	CefRuntimeRect Regions[MaxRegions];

	if ((int)dirtyRects.size() > MaxRegions)
	{
		CefRuntimeRect &Region = Regions[0];
		Region.X = 0;
		Region.Y = 0;
		Region.Width = width;
		Region.Height = height;
		Callbacks->Repaint(1, &Region, buffer, width, height);
	}
	else
	{
		int NumRegions = 0;

		for (RectList::const_iterator it = dirtyRects.begin(); it != dirtyRects.end(); ++it)
		{
			CefRuntimeRect &Region = Regions[NumRegions++];
			const CefRect &Rect = *it;

			Region.X = Rect.x;
			Region.Y = Rect.y;
			Region.Width = Rect.width;
			Region.Height = Rect.height;
		}

		Callbacks->Repaint(NumRegions, Regions, buffer, width, height);
	}

	Callbacks->LeaveCriticalSection();
}