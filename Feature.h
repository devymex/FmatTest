
const long SIFT_IMG_BORDER = 5;
const long SIFT_DESC_LEN = (128 * 3);

struct FEATURE2D
{
	cv::KeyPoint kp;
	byte desc[SIFT_DESC_LEN];
};

struct IMGFEATS
{
	std::vector<FEATURE2D> feats;
	cv::Mat desc;
	cv::flann::Index tree;
};

long ExtractFeatures(const cv::Mat &img, std::vector<FEATURE2D> &feats);

void BlockFeature(const cv::Mat &img, const cv::Size &blkSize,
				  std::vector<FEATURE2D> &feats);

void BuildDescMat(const std::vector<FEATURE2D> &feats, cv::Mat &matDesc);

void BuildImgFeats(const cv::Mat &img, IMGFEATS &imgFeat);

void SaveFeats(const std::string &strFile, const IMGFEATS &imgFeat);

void LoadFeats(const std::string &strFile, IMGFEATS &imgFeat);
