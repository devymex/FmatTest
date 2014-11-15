
struct MATCH
{
	long nIdx1;
	long nIdx2;
	cv::Point2f pt1;
	cv::Point2f pt2;
	float fDist;
};

bool operator < (const cv::Point2f &pt1, const cv::Point2f &pt2);

bool operator < (const MATCH &match1, const MATCH &match2);

bool operator == (const MATCH &match1, const MATCH &match2);

void MatchFeats(const IMGFEATS &imgFeats1, const IMGFEATS &imgFeats2,
				std::vector<MATCH> &matches);

void SaveMatches(const std::string &strFile, const std::vector<MATCH> &matches);

void LoadMatches(const std::string &strFile, std::vector<MATCH> &matches);
