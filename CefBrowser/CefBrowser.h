
// CefBrowser.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.
#include "ClientApp.h"

// CCefBrowserApp:
// �� Ŭ������ ������ ���ؼ��� CefBrowser.cpp�� �����Ͻʽÿ�.
//

class CCefBrowserApp : public CWinApp
{
public:
	CCefBrowserApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PumpMessage();

// �����Դϴ�.
	BOOL m_bCEFInitialized;
	CefRefPtr<ClientApp> m_cefApp;

	DECLARE_MESSAGE_MAP()
};

extern CCefBrowserApp theApp;