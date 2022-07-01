
// CefBrowserDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "CefBrowser.h"
#include "CefBrowserDlg.h"
#include "afxdialogex.h"

#include "include/cef_app.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CCefBrowserDlg ��ȭ ����



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


// CCefBrowserDlg �޽��� ó����

BOOL CCefBrowserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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
	//���� �ڵ带 false�� �ؾ� CefShutdown(); �Լ��� ���������� ȣ���� ������.
	//������ true�� �ϸ� Hang-up �ɷ��� ���μ����� ������� ����.

	if (m_bRemortDebug == TRUE)
	{
		CString strLogPath;
		strLogPath.Format(_T("%s\\Log\\CEF_LOG.txt"), m_config.GetCurrentPath());
		CefString(&settings.log_file) = strLogPath;

		// �Ʒ� ������ �ϸ� ũ�� �������� http://localhost:19500 ȣ���ϸ� CSS/HTML ����� ������
		settings.remote_debugging_port = 19500;
		settings.log_severity = LOGSEVERITY_DEBUG;
	}
	else {
		// �Ʒ��� ���� ������ ���ƾ� debug.log ���� ����� ���� �� ������, ��ĩ �ܺο� ���α׷� ������ �����ϴ� ������ �ذ� �� �� ����.
		  // �Ʒ��� ���� �������� ������, debug.log ������ �����Ǿ�, J-vascript �Լ��� ����ȴ�.
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

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CCefBrowserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CCefBrowserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Test �ڵ忡�� ��Ƴ��� �ʾҴµ�, ������ ������ �ڵ�� �ְŵ�.
// �Ʒ� ó�� ���־�� VK_RETURN�� VK_TAB�� CefBrowser�� Key Event�� �Ѿ
BOOL CCefBrowserDlg::PreTranslateMessage(MSG* pMsg) 
{	
	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_TAB)
		return FALSE;

	return CDialog::PreTranslateMessage(pMsg);
}
