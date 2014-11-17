#include "stdafx.h"
#include "Util.h"
#include "Feature.h"

std::string GetTempTreeFileName()
{
	char szBuf[MAX_PATH];
	GetTempPathA(sizeof(szBuf), szBuf);
	GetTempFileNameA(szBuf, "tree_", NULL, szBuf);
	return std::string(szBuf);
}

struct GREATER_KP_RES
{
	bool operator()(const cv::KeyPoint &kp1, const cv::KeyPoint &kp2)
	{
		return kp1.response > kp2.response;
	}
};

long ExtractFeatures(const cv::Mat &img, long nMaxKpNum,
					 std::vector<FEATURE2D> &feats)
{
	if (img.type() != CV_8UC3)
	{
		return 0;
	}

	std::vector<cv::Mat> bgrImgs(3);
	cv::split(img, bgrImgs);

	cv::Mat imgH;
	cv::cvtColor(img, imgH, cv::COLOR_BGR2GRAY);

	std::vector<cv::Mat> fltImgs(3);
	for (long i = 0; i < 3; ++i)
	{
		bgrImgs[i].convertTo(fltImgs[i], CV_32FC1);
	}
	
	std::vector<cv::KeyPoint> kps;
	cv::Ptr<cv::KAZE> kaze = cv::KAZE::create(true, false, 1e-4f);
	kaze->detect(img, kps);

	if (kps.size() > nMaxKpNum)
	{
		std::sort(kps.begin(), kps.end(), GREATER_KP_RES());
		kps.erase(kps.begin() + nMaxKpNum, kps.end());
	}

	std::vector<cv::Mat> matDesc(3);
	for (long i = 0; i < 3; ++i)
	{
		kaze->compute(bgrImgs[i], kps, matDesc[i]);
	}

	feats.reserve(feats.size() + kps.size());
	for (long i = 0; i < kps.size(); ++i)
	{
		feats.push_back(FEATURE2D());
		FEATURE2D &lastFeat = feats.back();
		lastFeat.kp = kps[i];
		for (long j = 0; j < 3; ++j)
		{
			FEATURE2D::_DT *pDst = lastFeat.desc + j * FEATURE2D::DESC_MONO_LEN;
			memcpy(pDst, matDesc[j].ptr<float>(i), FEATURE2D::DESC_MONO_BYTES);
		}
	}
	return long(kps.size());
}

void BlockFeature(const cv::Mat &img, const cv::Size &blkSize,
				  std::vector<FEATURE2D> &feats)
{
	cv::Size blkCnt(
		(img.cols + blkSize.width - 1) / blkSize.width,
		(img.rows + blkSize.height - 1) / blkSize.height);

	int nBlkBorder = 1;

	for (long r = 0; r < blkCnt.height; ++r)
	{
		for (long c = 0; c < blkCnt.height; ++c)
		{
			int nXMin = c * blkSize.width - nBlkBorder;
			int nXMax = nXMin + blkSize.width + 2 * nBlkBorder;
			int nYMin = r * blkSize.height - nBlkBorder;
			int nYMax = nYMin + blkSize.height + 2 * nBlkBorder;
			LimitMin(nXMin, 0);
			LimitMin(nYMin, 0);
			LimitMax(nXMax, img.cols);
			LimitMax(nYMax, img.rows);

			cv::Rect roi(nXMin, nYMin, nXMax - nXMin, nYMax - nYMin);
			long nFeatCnt = ExtractFeatures(img(roi), 20, feats);
			auto iFeat = feats.rbegin();
			for (long i = 0; i < nFeatCnt; ++i, ++iFeat)
			{
				iFeat->kp.pt.x += roi.x;
				iFeat->kp.pt.y += roi.y;
			}
			TRACE(_T("%d, %d\n"), r, c);
		}
	}
}

void BuildDescMat(const std::vector<FEATURE2D> &feats, cv::Mat &matDesc)
{
	matDesc = cv::Mat((long)feats.size(), FEATURE2D::DESC_LEN, CV_32F);
	for (long i = 0; i < (long)feats.size(); ++i)
	{
		memcpy(matDesc.ptr(i), feats[i].desc, FEATURE2D::DESC_BYTE);
	}
}

void BuildImgFeats(const cv::Mat &img, IMGFEATS &imgFeat)
{
	cv::Size blkSize(500, 500);
	BlockFeature(img, blkSize, imgFeat.feats);
	BuildDescMat(imgFeat.feats, imgFeat.desc);
	imgFeat.tree.build(imgFeat.desc, cv::flann::KDTreeIndexParams());
}

void SaveFeats(const std::string &strFile, const IMGFEATS &imgFeat)
{
	std::ofstream outFile(strFile, std::ios::binary);
	long nFeatCnt = (long)imgFeat.feats.size();

	outFile.write((char*)&nFeatCnt, sizeof(nFeatCnt));
	for (const auto &i : imgFeat.feats)
	{
		outFile.write((char*)&i, sizeof(i));
	}

	if (imgFeat.desc.rows != nFeatCnt ||
		imgFeat.desc.cols != FEATURE2D::DESC_LEN)
	{
		throw 0;
	}

	cv::Size matSize = imgFeat.desc.size();
	if (imgFeat.desc.isContinuous())
	{
		matSize.width *= matSize.height;
		matSize.height = 1;
	}
	for (long i = 0; i < matSize.height; ++i)
	{
		outFile.write((char*)imgFeat.desc.ptr(i), matSize.width * sizeof(float));
	}

	//Write tree to a temp tree file
	std::string strTreeFileName = GetTempTreeFileName();
	imgFeat.tree.save(strTreeFileName);

	//Get tree file size
	std::ifstream treeFile(strTreeFileName, std::ios::binary);
	treeFile.seekg(0, std::ios_base::end);
	long nFileSize = (long)treeFile.tellg();
	treeFile.seekg(0, std::ios_base::beg);

	//Read temp tree file data to buffer and delete the temp file
	std::vector<char> fileBuf(nFileSize);
	treeFile.read(fileBuf.data(), nFileSize);
	treeFile.close();
	DeleteFileA(strTreeFileName.c_str());

	// Write buffer with its size to output file
	outFile.write((char*)&nFileSize, sizeof(nFileSize));
	outFile.write(fileBuf.data(), nFileSize);
}

void LoadFeats(const std::string &strFile, IMGFEATS &imgFeat)
{
	std::ifstream inFile(strFile, std::ios::binary);
	long nFeatCnt = 0;

	inFile.read((char*)&nFeatCnt, sizeof(nFeatCnt));
	imgFeat.feats.resize(nFeatCnt);
	for (const auto &i : imgFeat.feats)
	{
		inFile.read((char*)&i, sizeof(i));
	}

	imgFeat.desc = cv::Mat(nFeatCnt, FEATURE2D::DESC_LEN, CV_32F);
	cv::Size matSize = imgFeat.desc.size();
	if (imgFeat.desc.isContinuous())
	{
		matSize.width *= matSize.height;
		matSize.height = 1;
	}
	for (long i = 0; i < matSize.height; ++i)
	{
		inFile.read((char*)imgFeat.desc.ptr(i), matSize.width * sizeof(float));
	}

	long nFileSize;
	inFile.read((char*)&nFileSize, sizeof(nFileSize));
	std::vector<char> fileBuf(nFileSize);
	inFile.read(fileBuf.data(), nFileSize);

	//Write buffer to a temp tree file
	std::string strTreeFileName = GetTempTreeFileName();
	std::ofstream treeFile(strTreeFileName, std::ios::binary);
	treeFile.write(fileBuf.data(), nFileSize);
	treeFile.close();

	//Load tree from the temp file and delete the temp file.
	imgFeat.tree.load(imgFeat.desc, strTreeFileName);
	DeleteFileA(strTreeFileName.c_str());
}
