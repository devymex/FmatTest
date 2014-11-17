#include "stdafx.h"
#include "Feature.h"
#include "Match.h"

bool operator < (const cv::Point2f &pt1, const cv::Point2f &pt2)
{
	if (pt1.y == pt2.y)
	{
		return pt1.x < pt2.x;
	}
	return pt1.y < pt2.y;
}

bool operator < (const MATCH &match1, const MATCH &match2)
{
	if (match1.pt1 == match2.pt1)
	{
		return match1.pt2 < match2.pt2;
	}
	return match1.pt1 < match2.pt1;
}

bool operator == (const MATCH &match1, const MATCH &match2)
{
	return ((match1.pt1 == match2.pt1) && (match1.pt2 == match2.pt2));
}

void MatchFeats(const IMGFEATS &imgFeats1, const IMGFEATS &imgFeats2,
				std::vector<MATCH> &matches)
{

	cv::Mat matDist = cv::Mat::zeros(1, 2, CV_32F);
	cv::Mat matIdx = cv::Mat::zeros(1, 2, CV_32S);

	cv::flann::Index &tree1 = const_cast<cv::flann::Index &>(imgFeats1.tree);
	cv::flann::Index &tree2 = const_cast<cv::flann::Index &>(imgFeats2.tree);

	for (long i = 0; i < (long)imgFeats2.feats.size(); ++i)
	{
		tree1.knnSearch(imgFeats2.desc.row(i), matIdx, matDist, 2);
		if (matDist.at<float>(0) < 0.6 * matDist.at<float>(1))
		{
			long nIdx1 = matIdx.at<int>(0);
			tree2.knnSearch(imgFeats1.desc.row(nIdx1), matIdx, matDist, 2);

			float f1stDist = matDist.at<float>(0);
			float f2ndDist = 0.6f * matDist.at<float>(1);
			if (f1stDist < f2ndDist && matIdx.at<int>(0) == i)
			{
				MATCH curMatch;
				curMatch.nIdx1 = nIdx1;
				curMatch.nIdx2 = matIdx.at<int>(0);
				curMatch.fDist = f1stDist;
				curMatch.pt1 = imgFeats1.feats[curMatch.nIdx1].kp.pt;
				curMatch.pt2 = imgFeats2.feats[curMatch.nIdx2].kp.pt;
				matches.push_back(curMatch);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	matches.erase(std::unique(matches.begin(), matches.end()), matches.end());
}

void SaveMatches(const std::string &strFile, const std::vector<MATCH> &matches)
{
	std::ofstream outFile(strFile, std::ios::binary);
	long nCnt = (long)matches.size();

	outFile.write((char*)&nCnt, sizeof(nCnt));
	for (const auto &i : matches)
	{
		outFile.write((char*)&i, sizeof(i));
	}
}

void LoadMatches(const std::string &strFile, std::vector<MATCH> &matches)
{
	std::ifstream inFile(strFile, std::ios::binary);
	long nCnt = 0;

	inFile.read((char*)&nCnt, sizeof(nCnt));
	matches.resize(nCnt);

	for (const auto &i : matches)
	{
		inFile.read((char*)&i, sizeof(i));
	}
}


void DrawMatches(const cv::Mat &img1, const cv::Mat &img2,
				 const std::vector<MATCH> &matches, cv::Mat &imgOut)
{
	long nCols = img1.cols;
	long nRows = img1.rows;
	cv::Mat imgComb = cv::Mat::zeros(nRows, nCols * 2, img1.type());
	img1.copyTo(imgComb.colRange(0, nCols));
	img2.copyTo(imgComb.colRange(nCols, nCols * 2));
	if (imgOut.size != imgComb.size || imgOut.type() != imgComb.type())
	{
		imgOut = imgComb.clone();
	}

	for (long i = 0; i < (long)matches.size(); ++i)
	{
		const MATCH &curMatch = matches[i];
		cv::Point pt1 = curMatch.pt1;
		cv::Point pt2 = curMatch.pt2;
		pt2.x += nCols;
		cv::Scalar clr;
		if (matches.size() == 1)
		{
			clr = cv::Scalar(0, 0, 255);
		}
		else
		{
			clr = cv::Scalar(rand() % 255, rand() % 255, rand() % 255, 0);
		}
		cv::circle(imgOut, pt1, 5, clr, 2);
		cv::circle(imgOut, pt2, 5, clr, 2);
		cv::line(imgOut, pt1, pt2, clr, 2);
	}
}
