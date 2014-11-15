#include "stdafx.h"
#include "ImgBuf.h"

CImgBuf::CImgBuf()
	: m_dScale(1.0)
	, m_SrcBox(0.0, 0.0, 0.0, 0.0)
{
}

const cv::Mat& CImgBuf::GetImg()
{
	return m_OrgImg;
}

double CImgBuf::GetScale()
{
	return m_dScale;
}

void CImgBuf::SetImg(cv::Mat &img)
{
	m_OrgImg = img.clone();
}

void CImgBuf::SetScale(double dScale)
{
	if (m_OrgImg.empty() || dScale <= 0.0)
	{
		throw 0;
	}
	if (m_dScale != dScale)
	{
		m_dScale = dScale;
		UpdateBufBox();
		m_BufImg = cv::Mat();
	}
}

void CImgBuf::SetSrcBox(const RECTD &srcBox)
{
	if (m_OrgImg.empty())
	{
		throw 0;
	}
	if (srcBox.x < 0 || srcBox.width + srcBox.x > m_OrgImg.cols ||
		srcBox.y < 0 || srcBox.height + srcBox.y > m_OrgImg.rows)
	{
		throw 0;
	}
	if (srcBox.area() <= 0)
	{
		throw 0;
	}
	if (m_SrcBox != srcBox)
	{
		m_SrcBox = srcBox;
		UpdateBufBox();
		m_BufImg = cv::Mat();
	}
}

RECTD CImgBuf::GetSrcBox()
{
	return m_SrcBox;
}

void CImgBuf::GetBuf(cv::Mat &bufImg, cv::Point2d &begPos)
{
	if (m_BufImg.empty())
	{
		if (m_dScale == 1.0)
		{
			m_BufImg = m_OrgImg(m_BufBox).clone();
		}
		else
		{
			cv::resize(m_OrgImg(m_BufBox), m_BufImg, cv::Size(), m_dScale, m_dScale,
				(m_dScale > 1.0 ? cv::INTER_NEAREST : cv::INTER_LANCZOS4));
		}
		if (m_BufImg.empty())
		{
			throw 0;
		}
	}
	bufImg = m_BufImg;

	begPos.x = (m_SrcBox.x - m_BufBox.x) * m_dScale;
	begPos.y = (m_SrcBox.y - m_BufBox.y) * m_dScale;
}

cv::Point2d CImgBuf::Buf2Src(const cv::Point2d &bufPt)
{
	cv::Point2d srcPt;
	srcPt.x = bufPt.x / m_dScale + m_BufBox.x;
	srcPt.y = bufPt.y / m_dScale + m_BufBox.y;
	return srcPt;
}

void CImgBuf::UpdateBufBox()
{
	cv::Size2d scrSizeSrc;
	scrSizeSrc.width = ::GetSystemMetrics(SM_CXSCREEN) / m_dScale;
	scrSizeSrc.height = ::GetSystemMetrics(SM_CYSCREEN) / m_dScale;

	m_BufBox.x = (int)std::floor(m_SrcBox.x - scrSizeSrc.width);
	m_BufBox.y = (int)std::floor(m_SrcBox.y - scrSizeSrc.height);
	m_BufBox.width = (int)std::ceil(m_SrcBox.width + scrSizeSrc.width * 2.0);
	m_BufBox.height = (int)std::ceil(m_SrcBox.height + scrSizeSrc.height * 2.0);

	LimitMin(m_BufBox.x, 0);
	LimitMin(m_BufBox.y, 0);
	LimitMax(m_BufBox.width, m_OrgImg.cols - m_BufBox.x);
	LimitMax(m_BufBox.height, m_OrgImg.rows - m_BufBox.y);

	if (m_BufBox.area() == 0)
	{
		throw 0;
	}
}
