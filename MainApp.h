#pragma once

class CMainApp : public CWinApp
{
	DECLARE_MESSAGE_MAP()

public:
	CMainApp();
	virtual ~CMainApp();

protected:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	CWnd *m_pTmpWnd;
};

extern CMainApp theApp;
