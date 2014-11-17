
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

void MatchFeats(const struct IMGFEATS &imgFeats1,
				const struct IMGFEATS &imgFeats2,
				std::vector<MATCH> &matches);

void SaveMatches(const std::string &strFile, const std::vector<MATCH> &matches);

void LoadMatches(const std::string &strFile, std::vector<MATCH> &matches);

void DrawMatches(const cv::Mat &img1, const cv::Mat &img2,
				 const std::vector<MATCH> &matches, cv::Mat &imgOut);
