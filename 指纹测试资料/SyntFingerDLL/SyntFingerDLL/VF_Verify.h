#ifndef	__VFVERIFY_H__
#define __VFVERIFY_H__

#include "VF_Type.h"
#include "VF_Global.h"
#include "VF_Function.h"
#include "VF_GlobalMatch.h"
#include "VF_PatternMatch.h"


//
//	verify : 指纹比对
//
void	verify(FEATUREPTR lpFeature, FEATUREPTR lpTemplate, PMATCHRESULT lpMatchResult,
			   VF_FLAG matchMode)
{
/////////////////////////////////////////////////////////////////////////////
//	lpFeature: [in] 要比对的第一个指纹特征指针
//	lpTemplate: [in] 要比对的第二个指纹特征指针
//	lpMatchResult: [out] 比对结果指针
//	matchMode: [in] 比对模式
/////////////////////////////////////////////////////////////////////////////
	
	MATCHRESULT globalMatchResult;
	sint32		score = 0;
	
	//全局比对
	globalMatch(lpFeature, lpTemplate, &globalMatchResult, matchMode);
	//patternMatch(lpFeature, lpTemplate, &globalMatchResult, matchMode);
	*lpMatchResult = globalMatchResult;
	//如果匹配特征点数较少，则对比对结果相似度进行修正
	if(matchMode != VF_MATCHMODE_IDENTIFY && lpMatchResult->MMCount < 15)
	{
		score = (sint32)(lpMatchResult->Similarity*sin(lpMatchResult->MMCount*PI/30.0) + 0.5);
		lpMatchResult->Similarity = score;
	}

}


#endif

