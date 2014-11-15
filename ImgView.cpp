#include "stdafx.h"
#include "ImgBuf.h"
#include "ImgView.h"

BEGIN_MESSAGE_MAP(CImgView, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
END_MESSAGE_MAP()

CImgView::CImgView()
	: m_pMemDC(NULL)
	, m_pMemBmp(NULL)
	, m_pDrawDC(NULL)
	, m_pDrawBmp(NULL)
	, m_pImgBuf(NULL)
	, m_ViewMode(VM_FIT)
{
}

CImgView::~CImgView()
{
	delete m_pMemDC;
	delete m_pDrawDC;
	delete m_pMemBmp;
	delete m_pDrawBmp;
	delete m_pImgBuf;
}

BOOL CImgView::Create(DWORD dwStyle, const CRect &rc,
					  CWnd *pParent, HICON hIcon, int nShow)
{
	LPCTSTR lpClassName = ::AfxRegisterWndClass(
		0, //CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW),
		NULL,
		hIcon);

	CreateEx(
		0,					// dwExStyle
		lpClassName,		// lpszClassName
		_T("MainWndName"),	// lpszWindowName
		dwStyle,			// dwStyle
		rc,					// rect
		pParent,			// pParentWnd
		0					// nID
		);

	ShowWindow(nShow);
	UpdateWindow();

	m_pMemDC = new CDC;
	m_pDrawDC = new CDC;
	m_pMemBmp = new CBitmap;
	m_pDrawBmp = new CBitmap;
	m_pImgBuf = new CImgBuf;

	CDC *pDC = GetDC();
	m_pMemDC->CreateCompatibleDC(pDC);
	m_pDrawDC->CreateCompatibleDC(pDC);
	RecreateMemBmp(pDC);
	ReleaseDC(pDC);

	ClearBg();
	return TRUE;
}

void CImgView::ClearBg()
{
	CRect clientBox;
	GetClientRect(clientBox);
	CBitmap *pOldBm = m_pMemDC->SelectObject(m_pMemBmp);
	m_pMemDC->FillSolidRect(&clientBox, RGB(128, 128, 128));
	m_pMemDC->SelectObject(pOldBm);
}

void CImgView::SetImg(const cv::Mat &img, VIEW_MODE viewMode)
{
	cv::Size imgSize = img.size();
	if (imgSize.area() == 0)
	{
		throw 0;
	}

	cv::Mat tmpImg;
	switch (img.type())
	{
	case CV_8U:
		cv::cvtColor(img, tmpImg, cv::COLOR_GRAY2BGRA);
		break;
	case CV_8UC3:
		cv::cvtColor(img, tmpImg, cv::COLOR_BGR2BGRA);
		break;
	case CV_8UC4:
		tmpImg = img;
		break;
	default:
		throw 0;
	}
	m_pImgBuf->SetImg(tmpImg);

	m_ViewMode = viewMode;
	switch (viewMode)
	{
	case VM_FIT:
		FitWnd();
		break;
	case VM_FREE:
		break;
	}
}

void CImgView::FitWnd()
{
	const cv::Mat &img = m_pImgBuf->GetImg();
	if (img.empty())
	{
		throw 0;
	}

	CRect wndBox;
	GetClientRect(wndBox);
	if (wndBox.Width() == 0 || wndBox.Height() == 0)
	{
		return;
	}

	double dWndRatio = (double)wndBox.Width() / (double)wndBox.Height();
	double dImgRatio = (double)img.cols / (double)img.rows;
	if (dWndRatio < dImgRatio)
	{
		m_pImgBuf->SetScale((double)wndBox.Width() / (double)img.cols);
	}
	else
	{
		m_pImgBuf->SetScale((double)wndBox.Height() / (double)img.rows);
	}

	m_pImgBuf->SetSrcBox(RECTD(cv::Point(0, 0), img.size()));
	RebuildBuf();
}

void CImgView::SetView(double dScale, const cv::Point2d &begPos)
{
	m_pImgBuf->SetScale(dScale);

	CRect wndBox;
	GetClientRect(wndBox);
	
	RECTD srcBox;
	srcBox.x = begPos.x;
	srcBox.y = begPos.y;
	srcBox.width = wndBox.Width() / dScale;
	srcBox.height = wndBox.Height() / dScale;

	cv::Size orgSize = m_pImgBuf->GetImg().size();
	LimitMax(LimitMin(srcBox.x, 0.0), orgSize.width - 1.0);
	LimitMax(LimitMin(srcBox.y, 0.0), orgSize.height - 1.0);
	LimitMax(srcBox.width, orgSize.width - srcBox.x);
	LimitMax(srcBox.height, orgSize.height - srcBox.y);

	m_pImgBuf->SetSrcBox(srcBox);

	RebuildBuf();
}

void CImgView::OnPaint()
{
	CPaintDC dc(this);

	CRect wndBox;
	GetClientRect(wndBox);

	CBitmap *pOldBm = m_pMemDC->SelectObject(m_pMemBmp);
	dc.BitBlt(
		0,
		0,
		wndBox.Width(),
		wndBox.Height(),
		m_pMemDC,
		0,
		0,
		SRCCOPY
		);
	m_pMemDC->SelectObject(pOldBm);
}

BOOL CImgView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE; //return CWnd::OnEraseBkgnd(pDC);

	UNREFERENCED_PARAMETER(pDC);
}

LRESULT CImgView::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	CDC *pDC = GetDC();
	RecreateMemBmp(pDC);
	ReleaseDC(pDC);
	return 0;

	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
}

void CImgView::RecreateMemBmp(CDC *pDC)
{
	m_pMemBmp->DeleteObject();
	long cx = GetSystemMetrics(SM_CXSCREEN);
	long cy = GetSystemMetrics(SM_CYSCREEN);
	m_pMemBmp->CreateCompatibleBitmap(pDC, cx, cy);

	BITMAP bmInfo;
	m_pMemBmp->GetBitmap(&bmInfo);
	if (bmInfo.bmBitsPixel != 32)
	{
		throw 0;
	}
}

void CImgView::Refresh()
{
	ClearBg();

	CBitmap *pOldDrawBmp = m_pDrawDC->SelectObject(m_pDrawBmp);
	CBitmap *pOldMemBm = m_pMemDC->SelectObject(m_pMemBmp);
	m_pMemDC->BitBlt(
		m_DstBox.left,
		m_DstBox.top,
		m_DstBox.Width(),
		m_DstBox.Height(),
		m_pDrawDC,
		(int)m_BegPos.x,
		(int)m_BegPos.y,
		SRCCOPY
		);
	m_pMemDC->SelectObject(pOldMemBm);
	m_pDrawDC->SelectObject(pOldDrawBmp);

	Invalidate(FALSE);
}

void CImgView::RebuildBuf()
{
	cv::Mat bufImg;
	m_pImgBuf->GetBuf(bufImg, m_BegPos);
	m_BufSize = bufImg.size();

	m_pDrawBmp->DeleteObject();
	m_pDrawBmp->CreateBitmap(bufImg.cols, bufImg.rows, 1, 32, bufImg.data);

	GetClientRect(m_DstBox);
	CSize dstBoxSize = m_DstBox.Size();
	LimitMax(dstBoxSize.cx, (long)(m_BufSize.width - m_BegPos.x));
	LimitMax(dstBoxSize.cy, (long)(m_BufSize.height - m_BegPos.y));

	long nMarginX = (m_DstBox.Width() - dstBoxSize.cx) / 2;
	long nMarginY = (m_DstBox.Height() - dstBoxSize.cy) / 2;
	m_DstBox.left += nMarginX;
	m_DstBox.top += nMarginY;
	m_DstBox.right -= nMarginX;
	m_DstBox.bottom -= nMarginY;
}

void CImgView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_pImgBuf != NULL)
	{
		if (!m_pImgBuf->GetImg().empty())
		{
			if (m_ViewMode == VM_FIT)
			{
				FitWnd();
				Refresh();
			}
			else
			{
				SetView(m_pImgBuf->GetScale(), m_BegPos);
				Refresh();
			}
		}
	}
}

BOOL CImgView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CPoint wndCursor = pt;
	ScreenToClient((LPPOINT)&wndCursor);
	
	CPoint dstCursor = wndCursor - m_DstBox.TopLeft();

	cv::Point2d bufCursor;
	bufCursor.x = dstCursor.x + m_BegPos.x;
	bufCursor.y = dstCursor.y + m_BegPos.y;

	cv::Point2d srcCursor = m_pImgBuf->Buf2Src(bufCursor);
	LimitMax(LimitMin(srcCursor.x, 0.0), (double)m_pImgBuf->GetImg().cols);
	LimitMax(LimitMin(srcCursor.y, 0.0), (double)m_pImgBuf->GetImg().rows);

	double dNewScale = m_pImgBuf->GetScale() * (zDelta > 0 ? 1.5 : 1 / 1.5);

	CRect wndBox;
	GetClientRect(wndBox);

	cv::Point2d srcBeg;
	srcBeg.x = srcCursor.x - (wndCursor.x - wndBox.left) / dNewScale;
	srcBeg.y = srcCursor.y - (wndCursor.y - wndBox.top) / dNewScale;
	SetView(dNewScale, srcBeg);

	Refresh();

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CImgView::OnMouseMove(UINT nFlags, CPoint point)
{
	static CPoint prevPt;
	if (nFlags & MK_MBUTTON)
	{
		CPoint moveDist = point - prevPt;
		m_BegPos.x -= moveDist.x;
		m_BegPos.y -= moveDist.y;
		LimitMax(LimitMin(m_BegPos.x, 0.0),
			(double)m_BufSize.width - m_DstBox.Width());
		LimitMax(LimitMin(m_BegPos.y, 0.0),
			(double)m_BufSize.height - m_DstBox.Height());
		Refresh();
	}
	prevPt = point;
	CWnd::OnMouseMove(nFlags, point);
}

void CImgView::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	CWnd::OnMButtonDown(nFlags, point);
}

void CImgView::OnMButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	SetView(m_pImgBuf->GetScale(), m_pImgBuf->Buf2Src(m_BegPos));

	CWnd::OnMButtonUp(nFlags, point);
}
