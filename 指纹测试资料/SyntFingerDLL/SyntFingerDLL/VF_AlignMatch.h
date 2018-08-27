#ifndef	__VFALIGNMATCH_H__
#define __VFALIGNMATCH_H__

#include "VF_Type.h"
#include "VF_Global.h"
#include "VF_Function.h"

// 为节省时间，短距离的两点间距离采用查表法
// DisTbl[m][n] = (int)(sqrt(m*m+n*n)+0.5)
sint32  DisTbl[10][10] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
	1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
	2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 
	3, 3, 3, 4, 5, 5, 6, 7, 8, 9, 
	4, 4, 4, 5, 5, 6, 7, 8, 8, 9, 
	5, 5, 5, 5, 6, 7, 7, 8, 9, 10, 
	6, 6, 6, 6, 7, 7, 8, 9, 10, 10, 
	7, 7, 7, 7, 8, 8, 9, 9, 10, 11, 
	8, 8, 8, 8, 8, 9, 10, 10, 11, 12, 
	9, 9, 9, 9, 9, 10, 10, 11, 12, 12
};

//
//	align: 将指纹特征按一定的角度和位置偏移进行坐标变换
//
void align(FEATUREPTR lpFeature, FEATUREPTR lpAlignedFeature, MINUTIAPTR lpFeatureCore,
				  sint32 rotation, sint32 transx, sint32 transy)
{
///////////////////////////////////////////////////////////////////////
//  lpFeature:	[in] 要变换的指纹特征
//  lpAlignedFeature:	[out] 进行坐标变换后的指纹特征
//  lpFeatureCore:	[in] 旋转变换的中心特征点
//  rotation:	[in] 旋转角度
//  transx:	[in] 水平偏移
//  transy: [in] 垂直偏移
///////////////////////////////////////////////////////////////////////

	sint32  i;
	sint32  x, y;
	sint32  cx, cy;
	double  rota, sinv, cosv;

	// 复制整个结构信息
	*lpAlignedFeature = *lpFeature;
	
	// 坐标转换的中心点坐标
	cx = lpFeatureCore->x;
	cy = lpFeatureCore->y;
	// 旋转的弧度
	rota = rotation/EPI;
	// 旋转弧度的sin值
	sinv = sin(rota);
	// 旋转弧度的cos值
	cosv = cos(rota);
	for(i = 0; i < lpFeature->MinutiaNum; i++)
	{
		x = lpFeature->MinutiaArr[i].x;
		y = lpFeature->MinutiaArr[i].y;
		//  坐标转换后的新坐标
		lpAlignedFeature->MinutiaArr[i].x = (sint32)(cx + cosv*(x-cx) - sinv*(y-cy) + transx + 0.5);
		lpAlignedFeature->MinutiaArr[i].y = (sint32)(cy + sinv*(x-cx) + cosv*(y-cy) + transy + 0.5);
		// 旋转后特征点的新方向
		lpAlignedFeature->MinutiaArr[i].Direction = (lpFeature->MinutiaArr[i].Direction + rotation) % 360;
	}

}

//
// alignMatch: 两个坐标系对齐的指纹特征进行比对
//
void alignMatch(FEATUREPTR lpAlignFeature, FEATUREPTR lpTemplate, 
				PMATCHRESULT lpMatchResult, VF_FLAG matchMode)
{
	sint32	i, j;
	uint8	flagA[MAXMINUTIANUM];	// 标记lpAlignFeature中特征点是否已经有匹配对象
	uint8	flagT[MAXMINUTIANUM];	// 标记lpTemplate中特征点是否已经有匹配对象
	sint32	x1, y1, x2, y2;
	sint32  dis, angle;
	sint32  score, matchNum, s;
	sint32	num1, num2;
	sint32  step = 1;
	num1 = lpAlignFeature->MinutiaNum;
	num2 = lpTemplate->MinutiaNum;
	// 标记清零
	memset(flagA, 0, MAXMINUTIANUM);
	memset(flagT, 0, MAXMINUTIANUM);

	score = 0;		// 总分清零
	matchNum = 0;	// 匹配特征点数清零



	// 相同类型特征点的匹配分数
	for(i = 0; i < lpTemplate->MinutiaNum; i++)
	{
		if(flagT[i])	// 是否已有匹配对象
			continue;
		for(j = 0; j < lpAlignFeature->MinutiaNum; j++)
		{
			if(flagA[j])	// 是否已有匹配对象
				continue;
			// 特征点类型是否相同
			if(lpTemplate->MinutiaArr[i].Type != lpAlignFeature->MinutiaArr[j].Type)
				continue;
			// 特征点方向夹角
			angle = AngleAbs360(lpTemplate->MinutiaArr[i].Direction, 
								lpAlignFeature->MinutiaArr[j].Direction);
			// 夹角>=10则不匹配
			if(angle >= 10)
				continue;
			x1 = lpTemplate->MinutiaArr[i].x;
			y1 = lpTemplate->MinutiaArr[i].y;
			x2 = lpAlignFeature->MinutiaArr[j].x;
			y2 = lpAlignFeature->MinutiaArr[j].y;
			// 水平距离>=10则不匹配
			if(abs(x1-x2) >= 10)
				continue;
			// 垂直距离>=10则不匹配
			if(abs(y1-y2) >= 10)
				continue;
			// 两特征点间的距离
			dis = DisTbl[abs(y1-y2)][abs(x1-x2)];
			// 距离>=10则不匹配
			if(dis >= 10)
				continue;
			// 对这两个特征点做标记，表示已经有匹配对象
			flagA[j] = 1;
			flagT[i] = 1;
			// 总分加上此两个特征点的匹配分数
			// 此表明dis,angle越大，分数越小
			score += (10-angle);
			score += (10-dis);
			// 匹配特征点数加一
			matchNum++;
			
			// 如果是快速比对模式
			if(matchMode == VF_MATCHMODE_IDENTIFY && matchNum >= 8)
			{
				// 计算相似度
				s = 4 * score * matchNum * MAXMINUTIANUM / ((num1 + num2) * ( num1 + num2));
				if(s > 100)  // 相似度足够大则返回比对结果
				{
					lpMatchResult->MMCount = matchNum;
					lpMatchResult->Rotation = 0;
					lpMatchResult->Similarity = s;
					lpMatchResult->TransX = 0;
					lpMatchResult->TransX = 0;
					return;
				}
			}

		}
	}

	if(matchMode != VF_MATCHMODE_IDENTIFY)
	{
		// 由于图像处理的误差导致可能端点处理成叉点或叉点处理成端点，假设概率为50%，计算
		// 此种情况的分数
		for(i = 0; i < lpTemplate->MinutiaNum; i++)
		{
			if(flagT[i])	// 是否已有匹配对象
				continue;
			for(j = 0; j < lpAlignFeature->MinutiaNum; j++)
			{
				if(flagA[j])	// 是否已有匹配对象
					continue;
				// 是否类型不同
				if(lpTemplate->MinutiaArr[i].Type == lpAlignFeature->MinutiaArr[j].Type)
					continue; 
				// 特征点方向夹角
				angle = AngleAbs360(lpTemplate->MinutiaArr[i].Direction, 
									lpAlignFeature->MinutiaArr[j].Direction);
				// 夹角>=10则不匹配
				if(angle >= 10)
					continue;
				x1 = lpTemplate->MinutiaArr[i].x;
				y1 = lpTemplate->MinutiaArr[i].y;
				x2 = lpAlignFeature->MinutiaArr[j].x;
				y2 = lpAlignFeature->MinutiaArr[j].y;

				// 水平距离>=10则不匹配
				if(abs(x1-x2) >= 10)
					continue;
				// 垂直距离>=10则不匹配
				if(abs(y1-y2) >= 10)
					continue;
				// 两特征点间的距离
				dis = DisTbl[abs(y1-y2)][abs(x1-x2)];
				// 距离>=10则不匹配
				if(dis >= 10)
					continue;
				// 对这两个特征点做标记，表示已经有匹配对象
				flagA[j] = 1;
				flagT[i] = 1;
				// 总分加上此两个特征点的匹配分数
				score += ((10-angle)/2);
				score += ((10-dis)/2);
				// 匹配特征点数加一
				matchNum++;

			}
		}								
	}
	// 计算相似度，返回比对结果
	s = 4 * score * matchNum * MAXMINUTIANUM / ((num1+num2)*(num1+num2));

	lpMatchResult->MMCount = matchNum;
	lpMatchResult->Rotation = 0;
	lpMatchResult->Similarity = s;
	lpMatchResult->TransX = 0;
	lpMatchResult->TransX = 0;

}



#endif