#include "stdafx.h"
#include "VF_Api.h"
#include "SyntFinger.h"


int __stdcall Add(int numa, int numb)
{
	return (numa + numb);
}

// Extract: 从指纹图像中提取指纹特征
int __stdcall Analyze(BYTE *lpImage, int Width, int Height, BYTE *lpFeature, int *lpSize)
{
	///////////////////////////////////////////////////////////////////////
	//  Width:	[in] 指纹图像宽度
	//  Height:	[in] 指纹图像高度
	//  lpImage:    [in] 指纹图像数据指针
	//  Resolution:	[in] 指纹图像分辨率，默认500
	//  lpFeature:	[out] 提取的指纹特征数据指针
	//  lpSize:	[out] 指纹特征数据大小
	///////////////////////////////////////////////////////////////////////

	// TODO: Add your implementation code here
	VF_RETURN	re;

	// 导入指纹图像数据
	VF_ImportFinger(lpImage, Width, Height);

	// 处理指纹图像，提取指纹特征
	re = VF_Process();
	if (re != VF_OK)
		return re;

	// 对指纹特征进行编码
	re = VF_FeatureEncode(&g_Feature, lpFeature, lpSize);
	if (re != VF_OK)
		return re;

	return 0;
}


// ExtractEx: 从指纹图像BMP文件中提取指纹特征
int __stdcall AnalyzeFromFile(LPCSTR lpszFileName, BYTE *lpFeature, int *lpSize)
{
	//  lpszFileName: [in] 指纹图像文件路径
	//  Resolution: [in] 指纹图像分辨率
	//  lpFeature: [out] 提取的指纹特征数据缓冲区指针
	//  lpSize: [out] 提取的指纹特征数据大小
	
	VF_RETURN	re;

	// 读取指纹图像数据
	re = VF_LoadFinger(lpszFileName);
	if (re != VF_OK)
		return re;

	// 处理指纹图像，提取指纹特征
	re = VF_Process();
	if (re != VF_OK)
		return re;

	// 对指纹特征进行编码
	re = VF_FeatureEncode(&g_Feature, lpFeature, lpSize);
	if (re != VF_OK)
		return re;

	return 0;
}

// VerifyMatch: 两个指纹特征间的比对
int __stdcall PatternMatch(BYTE *lpFeature1, BYTE *lpFeature2, int *lpScore)
{
	//	lpFeature1:		[in] 第一个指纹特征
	//	lpFeature2:		[in] 第二个指纹特征
	//	lpScore:		[out] 比对的相似度
	//	FastMode:		[in] 是否进行快速模式比对
	VF_RETURN	re1,re2;
	MATCHRESULT mr;
	FEATURE		feat1, feat2;

	// 第一个指纹特征的解码
	re1 = VF_FeatureDecode(lpFeature1, &feat1);
	if (re1 != VF_OK)
	{
		printf("图像1解码失败\n");
		return 0;
		//return re1;
	}

	// 第二个指纹特征的解码
	re2 = VF_FeatureDecode(lpFeature2, &feat2);
	if (re2 != VF_OK)
	{
		printf("图像2解码失败\n");
		return 0;
		//return re2;
	}

	*lpScore = 0;

	bool FastMode = true;

	if (FastMode)
	{
		// 快速模式的比对
		VF_VerifyMatch(&feat1, &feat2, &mr, VF_MATCHMODE_IDENTIFY);
	}
	else
	{
		// 精确模式的比对
		VF_VerifyMatch(&feat1, &feat2, &mr, VF_MATCHMODE_VERIFY);
	}

	// 匹配的相似度
	//*lpScore = mr.Similarity/10;
	*lpScore = mr.Similarity;
	/*if (mr.MMCount < 8)
	{
		*lpScore = 0;
	}
	else
	{
		*lpScore = mr.Similarity;
	}*/

	return 0;
}