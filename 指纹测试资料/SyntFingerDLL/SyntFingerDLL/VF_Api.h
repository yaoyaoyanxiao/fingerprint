#ifndef	__VFAPI_H__
#define __VFAPI_H__

#include "VF_Type.h"
#include "VF_Err.h"
#include "VF_Global.h"
#include "VF_FileIO.h"
#include "VF_Smooth.h"
#include "VF_Zoom.h"
#include "VF_GetOrientMap.h"
#include "VF_Divide.h"
#include "VF_Enhance.h"
#include "VF_Binary.h"
#include "VF_Thin.h"
#include "VF_GetMinutia.h"
#include "VF_Feature.h"
#include "VF_Verify.h"
#include "VF_Template.h"
#include "VF_GetSingulary.h"


//	VF_LoadFinger: 从文件中读取指纹数据
VF_RETURN	VF_LoadFinger(LPCSTR lpszFileName)
{
//	lpszFileName: [in] 指纹图像文件名
	sint32  bufferSize = 0;
	sint32  Width = 0;
	sint32  Height = 0;
	sint32  re;

	//获取需要内存空间大小，分配内存
	re = loadBitmap((char *)lpszFileName, NULL, &Width, &Height);
	if(re != 0)
	{
		return VF_LOADFINGER_ERR;
	}
	
	if(Width > MAXIMGW || Height > MAXIMGH)
	{
		return VF_LOADFINGER_ERR;
	}
	//	读取数据到图像处理缓冲区
	re = loadBitmap((char *)lpszFileName, g_lpOrgFinger, &Width, &Height);
	if(re != 0)
	{
		return VF_LOADFINGER_ERR;
	}
	
	// 设置处理的图像大小
	IMGW = Width;
	IMGH = Height;
	IMGSIZE = (sint32)(IMGW * IMGH);

	return	VF_OK;
}


//	VF_SaveFinger: 保存指纹数据为BMP文件
//
VF_RETURN	VF_SaveFinger(LPCSTR lpszFileName)
{
///////////////////////////////////////////////////////////////////////
//	lpszFileName: [in] 指纹图像文件名
///////////////////////////////////////////////////////////////////////

	sint32	re = 0;

	if(g_lpOrgFinger == NULL || IMGW <= 0 || IMGH <= 0)
	{
		return VF_SAVEFINGER_ERR;
	}
	// 将图像处理缓冲区内数据保存成BMP文件
	re = saveBitmap(g_lpOrgFinger, IMGW, IMGH, (char *)lpszFileName);

	if(re != 0)
	{
		return VF_SAVEFINGER_ERR;
	}

	return	VF_OK;
}
//
// VF_SaveOrientMap: 将方向场保存成BMP文件
//
VF_RETURN	VF_SaveOrientMap(LPCSTR lpszFileName)
{
///////////////////////////////////////////////////////////////////////
//	lpszFileName: [in] 方向场图像文件名
///////////////////////////////////////////////////////////////////////

	sint32	re = 0;

	if(g_lpOrient == NULL || IMGW <= 0 || IMGH <= 0)
	{
		return VF_SAVEFINGER_ERR;
	}
	// 将方向场数据保存为BMP图像文件
	re = saveBitmap(g_lpOrient, IMGW, IMGH, (char *)lpszFileName);

	if(re != 0)
	{
		return VF_SAVEFINGER_ERR;
	}

	return	VF_OK;
}


//
//	VF_ImportFinger: 从缓冲区中导入指纹数据
//
VF_RETURN	VF_ImportFinger(uint8  *lpImage, sint32 Width, sint32 Height)
{
///////////////////////////////////////////////////////////////////////
//	lpImage: [in] 指纹图像数据缓冲区
//  Width:	[in] 指纹图像宽度
//  Height:	[in] 指纹图像高度
///////////////////////////////////////////////////////////////////////

	if(Width > MAXIMGW || Height > MAXIMGH)
	{
		return VF_IMPORTFINGER_ERR;
	}
	// 设置处理的图像大小
	IMGW = Width;
	IMGH = Height;
	IMGSIZE = (sint32)(IMGW * IMGH);
	// 将图像数据拷贝到图像处理缓冲区
	memcpy((void *)g_lpOrgFinger, (void *)lpImage, IMGSIZE);

	return	VF_OK;
}

//
//	VF_FeatureEncode: 对指纹特征进行压缩编码
VF_RETURN VF_FeatureEncode(FEATUREPTR lpFeature, uint8 *lpData, sint32 *lpSize)
{
///////////////////////////////////////////////////////////////////////
//	lpFeature: [in] 要编码的指纹特征指针
//  lpData:	[out] 编码后的指纹特征数据缓冲区指针
//  lpSize:	[out] 编码后的指纹特征大小
///////////////////////////////////////////////////////////////////////

	sint32 re;

	// 压缩编码
	re = EncodeFeature(lpFeature, lpData);
	if(re == 0)
		return VF_FEATUREENCODE_ERR;

	*lpSize = re;

	return VF_OK;
}

//
//	VF_FeatureDecode: 对指纹特征进行解码
//
VF_RETURN VF_FeatureDecode(uint8 *lpData, FEATUREPTR lpFeature)
{
///////////////////////////////////////////////////////////////////////
//  lpData:	[in] 要解码的指纹特征数据缓冲区指针
//  lpFeature:	[out] 解码后的指纹特征
///////////////////////////////////////////////////////////////////////

	sint32 re;
	// 解码
	re = DecodeFeature(lpData, lpFeature);

	if(re != 0)
		return VF_FEATUREDECODE_ERR;

	return VF_OK;
}

//
// 对指纹图像进行处理，并提取指纹特征
//
VF_RETURN	VF_Process()
{
	sint32	re = 0;

	// 对指纹图像进行两次平滑，消除噪声
	smooth(g_lpOrgFinger, g_lpTemp, 1, 1);
	memcpy(g_lpOrgFinger, g_lpTemp, IMGSIZE);
	smooth(g_lpOrgFinger, g_lpTemp, 1, 1);
	memcpy(g_lpOrgFinger, g_lpTemp, IMGSIZE);
	
	// 利用图像低频部分计算方向场，提高速度
	zoomout();
	// 计算方向场
	getOrientMap(6);
	saveBitmap(g_lpOrient, IMGW, IMGH, "c:\\0.bmp");

	// 分割前景背景
	re = divide(12, 35);
	if(re != 0)
		return VF_PROCESS_ERR;
	saveBitmap(g_lpDivide, IMGW, IMGH, "c:\\00.bmp");

	// 两次定向滤波
	orientFilter();
//	orientFilter();
	saveBitmap(g_lpOrgFinger, IMGW, IMGH, "c:\\1.bmp");

	// 二值化
	binary();
	saveBitmap(g_lpOrgFinger, IMGW, IMGH, "c:\\2.bmp");

	// 二值化图像去噪声
	binaryClear();
	saveBitmap(g_lpOrgFinger, IMGW, IMGH, "c:\\3.bmp");

	// 清除背景
	clearEdge();
	saveBitmap(g_lpOrgFinger, IMGW, IMGH, "c:\\4.bmp");

	// 细化
	thin();
	// 细化图像去短棒和毛刺
	thinClear(12);
	saveBitmap(g_lpOrgFinger, IMGW, IMGH, "c:\\5.bmp");

	// 提取指纹特征
	re = getMinutia();
	int re2 = getSingular(g_lpOrient, IMGW, IMGH, -1);
	//int re3 = getSingular(g_lpOrient, IMGW, IMGH, 1);

	if(re != 0 && re2 != 0)
		return VF_PROCESS_ERR;

	return VF_OK;
}

//
// VF_Extract: 对指纹特征进行压缩编码并输出
VF_RETURN VF_Extract(uint8 *lpData, sint32 *lpSize)
{
//  lpData:	[out] 输出的指纹特征数据缓冲区指针
//  lpSize:	[out] 输出的指纹特征大小

	VF_RETURN  re;
	// 对提取的指纹特征进行编码并输出
	re = VF_FeatureEncode(&g_Feature, lpData, lpSize);
	
	if(re != VF_OK)
		return VF_EXTRACT_ERR;

	return VF_OK;
}


//
// VF_VerifyMatch: 两个指纹特征的比对
BOOL	VF_VerifyMatch(FEATUREPTR lpFeature1, FEATUREPTR lpFeature2, 
						   PMATCHRESULT lpMatchResult, VF_FLAG matchMode)
{
///////////////////////////////////////////////////////////////////////
//  lpFeature1:	[in] 第一个指纹特征
//  lpFeature2:	[in] 第二个指纹特征
//  lpMatchResult:	[out] 比对结果
//  matchMode:	[in] 比对模式
///////////////////////////////////////////////////////////////////////

	// 比对
	verify(lpFeature1, lpFeature2, lpMatchResult, matchMode);
	
	// 默认阈值为50。相似度>=50则认为比对成功，否则比对失败,
	// 一般情况下，在35以上即可认为相似, 为安全起见，最好在50-100
	//if (lpMatchResult->Similarity >= 30)//原来为50
	//	return TRUE;
	//else
	//	return FALSE;

	return TRUE;
}



#endif
