#pragma once

#include "Util.h"

class CImgBuf
{
public:
	CImgBuf();

	void GetBuf(cv::Mat &imgBuf, cv::Point2d &begPos);

	const cv::Mat& GetImg();

	double GetScale();

	RECTD GetSrcBox();

	void SetSrcBox(const RECTD &srcBox);

	void SetImg(cv::Mat &img);
	
	void SetScale(double dScale);

	cv::Point2d Buf2Src(const cv::Point2d &bufPt);

private:

	void UpdateBufBox();

	double m_dScale;

	RECTD m_SrcBox;

	cv::Rect m_BufBox;

	cv::Mat m_OrgImg;

	cv::Mat m_BufImg;
};
