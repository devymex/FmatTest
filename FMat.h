#pragma once

typedef std::vector<cv::Point2f> VEC_PT2F;

class RANDSELECTOR
{
private:
	std::vector<long> m_indices;

public:
	RANDSELECTOR(long nCnt)
	{
		for (long i = 0; i < nCnt; ++i)
		{
			m_indices.push_back(i);
		}
	}
	void RandSel(long nCnt)
	{
		long nAllCnt = (long)m_indices.size();
		for (long i = 0; i < nCnt; ++i)
		{
			long nRand = rand() % (nAllCnt - i);
			std::swap(m_indices[nRand], m_indices[nAllCnt - i - 1]);
		}
	}
	long GetSel(long nIdx)
	{
		return *(m_indices.end() - nIdx - 1);
	}
	long GetOther(long nIdx)
	{
		return *(m_indices.begin() + nIdx);
	}
};

class ROULETTESELECTOR
{
private:
	std::vector<double> m_Roulette;
};

void GetKps(const std::vector<struct MATCH> &matches,
			VEC_PT2F &kps1, VEC_PT2F &kps2);

double PointLineDist(const cv::Point2d &pt, const cv::Point3d &line);

void ReprojError(const VEC_PT2F &kps1, const VEC_PT2F &kps2,
				 const cv::Mat &matF, std::vector<double> &errs);
