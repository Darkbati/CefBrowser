
// CefBrowserDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "CefBrowser.h"
#include "CefBrowserDlg.h"
#include "afxdialogex.h"

#include "include/cef_app.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCefBrowserDlg 대화 상자



CCefBrowserDlg::CCefBrowserDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCefBrowserDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCefBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCefBrowserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CCefBrowserDlg 메시지 처리기

BOOL CCefBrowserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// Enable High-DPI support on Windows 7 or newer.
	CefEnableHighDPISupport();

	// initialize CEF.
	theApp.m_cefApp = new ClientApp();

	// get arguments
	CefMainArgs main_args(GetModuleHandle(NULL));

	// Execute the secondary process, if any.
	int exit_code = CefExecuteProcess(main_args, theApp.m_cefApp.get(), NULL);
	if (exit_code >= 0)
		return exit_code;

	// setup settings
	CString szCEFCache;
	CString szPath;
	INT nLen = GetTempPath( 0, NULL ) + 1;
	GetTempPath( nLen, szPath.GetBuffer( nLen ));

	// save path
	szCEFCache.Format( _T("%scache\0\0"), szPath );

	CefSettings settings;
	//settings.no_sandbox = TRUE;
	settings.multi_threaded_message_loop = FALSE;
	//위에 코드를 false로 해야 CefShutdown(); 함수를 정상적으로 호출이 가능함.
	//위에를 true로 하면 Hang-up 걸려서 프로세스가 종료되지 않음.

	if (m_bRemortDebug == TRUE)
	{
		CString strLogPath;
		strLogPath.Format(_T("%s\\Log\\CEF_LOG.txt"), m_config.GetCurrentPath());
		CefString(&settings.log_file) = strLogPath;

		// 아래 설정을 하면 크롬 브라우저에 http://localhost:19500 호출하면 CSS/HTML 디버깅 가능함
		settings.remote_debugging_port = 19500;
		settings.log_severity = LOGSEVERITY_DEBUG;
	}
	else {
		// 아래와 같이 설정해 놓아야 debug.log 파일 기록을 막을 수 있으며, 자칫 외부에 프로그램 구조를 노출하는 문제를 해결 할 수 있음.
		  // 아래와 같이 설정하지 않으면, debug.log 파일이 생성되어, J-vascript 함수가 노출된다.
		settings.log_severity = LOGSEVERITY_DISABLE;
	}

	CefString(&settings.cache_path) = szCEFCache;

	void* sandbox_info = NULL;
#if CEF_ENABLE_SANDBOX
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	//CEF Initiaized
	theApp.m_bCEFInitialized = CefInitialize(main_args, settings, theApp.m_cefApp.get(), sandbox_info);

	// get rect
	CRect rect;
	GetClientRect( rect );

	// set browser
	if( !theApp.m_cefApp->CreateBrowser( GetSafeHwnd(), rect, _T("https://www.google.com/") ))
	{
		AfxMessageBox(_T("Failed to create CEF Browser Window. Applications is exiting"));
		return FALSE;
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CCefBrowserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCefBrowserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCefBrowserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Test 코드에는 담아놓지 않았는데, 이전에 보내준 코드는 있거든.
// 아래 처럼 해주어야 VK_RETURN과 VK_TAB이 CefBrowser로 Key Event가 넘어감
BOOL CCefBrowserDlg::PreTranslateMessage(MSG* pMsg) 
{	
	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_TAB)
		return FALSE;

	return CDialog::PreTranslateMessage(pMsg);
}
