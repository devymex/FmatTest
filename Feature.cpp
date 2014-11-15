#include "stdafx.h"
#include "Feature.h"

std::string GetTempTreeFileName()
{
	char szBuf[MAX_PATH];
	GetTempPathA(sizeof(szBuf), szBuf);
	GetTempFileNameA(szBuf, "tree_", NULL, szBuf);
	return std::string(szBuf);
}

long ExtractFeatures(const cv::Mat &img, std::vector<FEATURE2D> &feats)
{
	cv::Ptr<cv::ORB> orb = cv::ORB::create();
	std::vector<cv::KeyPoint> kps;
	cv::Mat matDesc;
	orb->detectAndCompute(img, cv::noArray(), kps, matDesc);

	feats.reserve(feats.size() + kps.size());
	for (long i = 0; i < kps.size(); ++i)
	{
		feats.push_back(FEATURE2D());
		FEATURE2D &lastFeat = feats.back();
		lastFeat.kp = kps[i];
		for (long j = 0; j < SIFT_DESC_LEN; ++j)
		{
			/*****************************************************************/
			// ensure each elem of sift desc is limited with [0-255]
			float test = matDesc.at<float>(i, j);
			if (test > 255 || test < 0)
			{
				throw 0;
			}
			/*****************************************************************/

			lastFeat.desc[j] = static_cast<byte>(matDesc.at<float>(i, j));
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

	for (long r = 0; r < blkCnt.height; ++r)
	{
		for (long c = 0; c < blkCnt.height; ++c)
		{
			cv::Range colRange, rowRange;

			//inflate block for sift border
			colRange.start = c * blkSize.width - SIFT_IMG_BORDER;
			rowRange.start = r * blkSize.height - SIFT_IMG_BORDER;
			colRange.end = (c + 1) * blkSize.width + SIFT_IMG_BORDER;
			rowRange.end = (r + 1) * blkSize.height + SIFT_IMG_BORDER;

			//
			if (colRange.start < 0) colRange.start = 0;
			if (rowRange.start < 0) rowRange.start = 0;
			if (colRange.end > img.cols) colRange.end = img.cols;
			if (rowRange.end > img.rows) rowRange.end = img.rows;

			long nFeatCnt = ExtractFeatures(img(rowRange, colRange), feats);
			auto iFeat = feats.rbegin();
			auto iEnd = iFeat + nFeatCnt;
			for (; iFeat < iEnd; ++iFeat)
			{
				iFeat->kp.pt.x += colRange.start;
				iFeat->kp.pt.y += rowRange.start;
			}
			std::cout << r << ',' << c << std::endl;
		}
	}
}

void BuildDescMat(const std::vector<FEATURE2D> &feats, cv::Mat &matDesc)
{
	matDesc.release();
	matDesc.create((long)feats.size(), SIFT_DESC_LEN, CV_32F);
	for (long i = 0; i < (long)feats.size(); ++i)
	{
		for (long j = 0; j < SIFT_DESC_LEN; ++j)
		{
			matDesc.at<float>(i, j) = feats[i].desc[j];
		}
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

	if (imgFeat.desc.rows != nFeatCnt || imgFeat.desc.cols != SIFT_DESC_LEN)
	{
		throw 0;
	}

	for (long i = 0; i < nFeatCnt; ++i)
	{
		for (long j = 0; j < SIFT_DESC_LEN; ++j)
		{
			byte byVal = static_cast<byte>(imgFeat.desc.at<float>(i, j));
			outFile.write((char*)&byVal, sizeof(byVal));
		}
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

	imgFeat.desc.create(nFeatCnt, SIFT_DESC_LEN, CV_32F);
	for (long i = 0; i < nFeatCnt; ++i)
	{
		for (long j = 0; j < SIFT_DESC_LEN; ++j)
		{
			byte byVal;
			inFile.read((char*)&byVal, sizeof(byVal));
			imgFeat.desc.at<float>(i, j) = byVal;
		}
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
