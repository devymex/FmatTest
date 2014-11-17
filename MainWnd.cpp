#include "stdafx.h"
#include "Feature.h"
#include "Match.h"
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

	std::string strPath = "D:\\Documents\\ComVis\\expts\\floor\\";

	cv::Mat img1 = cv::imread(strPath+ "IMG_4486.jpg");
	cv::Mat img2 = cv::imread(strPath+ "IMG_4487.jpg");

	sizeof(FEATURE2D);
	IMGFEATS imgFeats1, imgFeats2;
	//BuildImgFeats(img1, imgFeats1);
	//BuildImgFeats(img2, imgFeats2);
	//SaveFeats(strPath + "IMG_4486.ft", imgFeats1);
	//SaveFeats(strPath + "IMG_4487.ft", imgFeats2);

	LoadFeats(strPath + "IMG_4486.ft", imgFeats1);
	LoadFeats(strPath + "IMG_4487.ft", imgFeats2);

	std::vector<MATCH> matches;
	//MatchFeats(imgFeats1, imgFeats2, matches);
	//SaveMatches(strPath + "matches", matches);
	LoadMatches(strPath + "matches", matches);

	//cv::Mat imgOut;
	//DrawMatches(img1, img2, matches, imgOut);
	//cv::imwrite(strPath + "out.jpg", imgOut);



	//m_pImgView->SetImg(img, CImgView::VM_FREE);
	//m_pImgView->SetView(1.0, cv::Point2d(0, 0));
	//m_pImgView->Refresh();

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
