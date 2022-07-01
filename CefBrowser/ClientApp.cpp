#include "stdafx.h"
#include "ClientApp.h"
#include "ClientHandler.h"

#include "ClientUtil.h"

ClientApp::ClientApp(void)
{
}

ClientApp::ClientApp(HWND hWnd)
{

}


ClientApp::~ClientApp(void)
{
}


void ClientApp::OnContextInitialized()
{
	REQUIRE_UI_THREAD()

	// SimpleHandler implements browser-level callbacks.
	m_cefHandler = new ClientHandler();
}


BOOL ClientApp::CreateBrowser(HWND hWnd, CRect rect, LPCTSTR pszURL)
{
	// settings
	CefBrowserSettings settings;
	CefWindowInfo info;
	
	// set browser as child
	info.SetAsChild( hWnd, rect );

	// create browser window
	return CefBrowserHost::CreateBrowser( info, m_cefHandler.get(), pszURL, settings, NULL );
}
