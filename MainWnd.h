#pragma once

class CMainWnd : public CWnd
{
	DECLARE_MESSAGE_MAP()

private:
	class CImgView *m_pImgView;

public:
	CMainWnd();
	virtual ~CMainWnd();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
