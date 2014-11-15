#include "stdafx.h"
#include "MainWnd.h"
#include "MainApp.h"

BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
END_MESSAGE_MAP()

CMainApp theApp;

CMainApp::CMainApp()
{
	// Support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: Set CompanyName.ProductName.SubProduct.VersionInformation here
	SetAppID(_T("Comp.Prod.SubProd.Ver"));
}

CMainApp::~CMainApp()
{
	delete m_pTmpWnd;
}

BOOL CMainApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	EnableTaskbarInteraction(FALSE);

	//::AfxEnableControlContainer();
	//::AfxInitRichEdit2();
	//::AfxOleInit();

	m_pTmpWnd = new CMainWnd;
	m_pMainWnd = m_pTmpWnd;

	// TODO: Change parameters of window class here
	LPCTSTR lpClassName = ::AfxRegisterWndClass(
		CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH) ::GetStockObject(WHITE_BRUSH),
		::LoadIcon(NULL, IDI_APPLICATION));

	// TODO: Change parameters of window creation here
	m_pMainWnd->CreateEx(
		0,
		lpClassName,
		_T("MainWndName"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		0);

	// Show and update the main window
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

int CMainApp::ExitInstance()
{
	//::AfxOleTerm(FALSE);
	return CWinApp::ExitInstance();
}
