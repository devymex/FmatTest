#include "stdafx.h"
#include "Match.h"
#include "FMat.h"

void GetKps(const std::vector<struct MATCH> &matches,
			VEC_PT2F &kps1, VEC_PT2F &kps2)
{
	kps1.clear();
	kps2.clear();
	for (auto i : matches)
	{
		kps1.push_back(i.pt1);
		kps2.push_back(i.pt2);
	}
}

double PointLineDist(const cv::Point2d &pt, const cv::Point3d &line)
{
	double den = std::sqrt(line.x * line.x + line.y * line.y);
	double rem = pt.x * line.x + pt.y * line.y + line.z;
	return (std::abs(rem) / den);
}

void ReprojError(const VEC_PT2F &kps1, const VEC_PT2F &kps2,
				 const cv::Mat &matF, std::vector<double> &errs)
{
	if (kps1.size() != kps2.size())
	{
		throw 0;
	}
	errs.clear();
	long nPtNum = (long)kps1.size();
	std::vector<cv::Point3f> lines1, lines2;
	cv::computeCorrespondEpilines(kps1, 1, matF, lines1);
	cv::computeCorrespondEpilines(kps2, 2, matF, lines2);
	for (long i = 0; i < nPtNum; ++i)
	{
		double dErr1 = PointLineDist(kps1[i], lines2[i]);
		double dErr2 = PointLineDist(kps2[i], lines1[i]);
		errs.push_back(std::max(dErr1, dErr2));
	}
}

void GetOrder(const std::vector<double> &vals, std::vector<long> &order)
{
	struct MATCHSCORE
	{
		long nId;
		double dScore;
		bool operator < (const MATCHSCORE &other)
		{
			return dScore < other.dScore;
		}
	};
	std::vector<MATCHSCORE> matchScores;
	for (long j = 0; j < (long)vals.size(); ++j)
	{
		MATCHSCORE ms = {j, vals[j]};
		matchScores.push_back(ms);
	}
	std::sort(matchScores.begin(), matchScores.end());

	order.clear();
	for (long i = 0; i < (long)vals.size(); ++i)
	{
		order.push_back(matchScores[i].nId);
	}
}

void Ransasc(const VEC_PT2F &kps1, const VEC_PT2F &kps2,
			 std::vector<double> &scores)
{
	if (kps1.size() != kps2.size())
	{
		throw 0;
	}
	long nPtNum = (long)kps1.size();
	long nOrderNotChangeCnt = 0;

	std::vector<cv::Point2f> randEight1, randEight2;
	std::vector<std::vector<double>> errVec(nPtNum);
	std::vector<long> prevOrder;
	std::vector<long> randIds;

	scores.clear();
	scores.resize(nPtNum, 0.0);
	for(long i = 0; nOrderNotChangeCnt < 5; ++i)
	{
		randEight1.clear();
		randEight2.clear();
		randIds.clear();

		// Select random 8 points
		RANDSELECTOR rs(nPtNum);
		rs.RandSel(8);
		for (long j = 0; j < 8; ++j)
		{
			randIds.push_back(rs.GetSel(j));
		}
		for (auto j : randIds)
		{
			randEight1.push_back(kps1[j]);
			randEight2.push_back(kps1[j]);
		}

		// Calculate F matrix and reprojection errors
		cv::Mat matF = cv::findFundamentalMat(randEight1, randEight2,
			cv::FM_8POINT);
		ReprojError(kps1, kps2, matF, scores);
		for (long j = 0; j < nPtNum; ++j)
		{
			double dErr = scores[j];
			auto k = std::upper_bound(errVec[j].begin(), errVec[j].end(), dErr);
			errVec[j].insert(k, dErr);
		}

		// Check if order changed.
		for (long j = 0; j < nPtNum; ++j)
		{
			double dMedian = errVec[j][errVec[j].size() / 2];
			scores[j] = M_LN10 / std::log(dMedian + 1.0);
		}
		std::vector<long> order;
		GetOrder(scores, order);
		if (prevOrder != order)
		{
			prevOrder = order;
			nOrderNotChangeCnt = 0;
		}
		else
		{
			++nOrderNotChangeCnt;
		}
	}
}
