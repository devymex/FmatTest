#pragma once

class CImgView : public CWnd
{
	DECLARE_MESSAGE_MAP();

public:
	enum VIEW_MODE {VM_FIT, VM_FREE};

private:
	class CImgBuf *m_pImgBuf;

	CDC *m_pDrawDC;
	CDC *m_pMemDC;
	CBitmap *m_pMemBmp;
	CBitmap *m_pDrawBmp;

	cv::Size m_BufSize;
	cv::Point2d m_BegPos;
	CRect m_DstBox;

	VIEW_MODE m_ViewMode;

public:
	CImgView();
	~CImgView();

	BOOL Create(DWORD dwStyle, const CRect &rc,
		CWnd *pParent = NULL, HICON hIcon = NULL, int nShow = SW_SHOW);

	void SetImg(const cv::Mat &img, VIEW_MODE viewMode);
	void FitWnd();
	void SetView(double dScale, const cv::Point2d &begPos);
	void Refresh();

protected:
	void ClearBg();
	void RebuildBuf();
	void RecreateMemBmp(CDC *pDC);
	void UpdateView();

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
};
