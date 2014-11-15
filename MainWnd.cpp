#include "stdafx.h"
#include "Feature.h"
#include "ImgView.h"
#include "MainWnd.h"

BEGIN_MESSAGE_MAP(CMainWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZING()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CMainWnd::CMainWnd()
	: m_pImgView(NULL)
{
}

CMainWnd::~CMainWnd()
{
	delete m_pImgView;
}

int CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}


	m_pImgView = new CImgView;

	CRect clientBox;
	GetClientRect(clientBox);
	clientBox.DeflateRect(10, 10);
	m_pImgView->Create(WS_CHILD | WS_BORDER, clientBox, this);
	m_pImgView->SetFocus();

	cv::Mat img = cv::imread("D:\\My Pictures\\hongkong\\IMG_4021.JPG");
	m_pImgView->SetImg(img, CImgView::VM_FREE);
	m_pImgView->SetView(1.0, cv::Point2d(0, 0));
	m_pImgView->Refresh();

	IMGFEATS imgFeats;
	BuildImgFeats(img, imgFeats);

	return 0;
}

void CMainWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	TRACE("SetCapture();\n");
	CWnd::OnRButtonDown(nFlags, point);
}

void CMainWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	CWnd::OnRButtonUp(nFlags, point);
}

BOOL CMainWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE)
	{
		CPoint cursorPos;
		GetCursorPos(&cursorPos);
		CWnd *pWnd = ChildWindowFromPoint(cursorPos);
		if (pWnd != NULL)
		{
			if (pWnd->GetParent() == this)
			{
				CWnd *pCurFocus = GetFocus();
				if (pCurFocus != pWnd)
				{
					pWnd->SetFocus();
					TRACE(_T("WM_MOUSEMOVE: %08x\n"), pMsg->time);
				}
			}
		}
	}
	if (pMsg->message == WM_MOUSEHWHEEL)
	{
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CMainWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CRect wndBox;
	GetClientRect(wndBox);
	wndBox.DeflateRect(CSize(10, 10));
	m_pImgView->SetWindowPos(NULL, wndBox.left, wndBox.top,
		wndBox.Width(), wndBox.Height(), SWP_NOZORDER);
}
