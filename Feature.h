

struct FEATURE2D
{
	typedef float _DT;
	enum {
		DESC_MONO_LEN = 128,
		DESC_MONO_BYTES = (DESC_MONO_LEN * sizeof(_DT)),
		DESC_LEN = (DESC_MONO_LEN * 3),
		DESC_BYTE = (DESC_LEN * sizeof(_DT)),
	};
	cv::KeyPoint kp;
	_DT desc[DESC_LEN];
};

struct IMGFEATS
{
	std::vector<FEATURE2D> feats;
	cv::Mat desc;
	cv::flann::Index tree;
};

long ExtractFeatures(const cv::Mat &img, long nMaxKpNum,
					 std::vector<FEATURE2D> &feats);

void BlockFeature(const cv::Mat &img, const cv::Size &blkSize,
				  std::vector<FEATURE2D> &feats);

void BuildDescMat(const std::vector<FEATURE2D> &feats, cv::Mat &matDesc);

void BuildImgFeats(const cv::Mat &img, IMGFEATS &imgFeat);

void SaveFeats(const std::string &strFile, const IMGFEATS &imgFeat);

void LoadFeats(const std::string &strFile, IMGFEATS &imgFeat);
