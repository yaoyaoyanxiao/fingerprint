#ifndef	__VFDIVIDE_H__
#define __VFDIVIDE_H__

#include "VF_Type.h"
#include "VF_Global.h"
#include "VF_Smooth.h"

//
// divide: 根据方向场幅值来分割指纹图像前景背景
//
sint32 divide(sint32  r, sint32 threshold)
{
///////////////////////////////////////////////////////////////////////
//	r: [in] 对幅值图像高度平滑滤波的滤波器半径
//  threshold: [in] 分割的阈值
///////////////////////////////////////////////////////////////////////
	sint32	x, y;
	sint32  num = 0;

	// 对方向场幅值图像进行高度平滑滤波
	smooth(g_lpDivide, g_lpTemp, r, 2);
	
	// 图像边缘均设置为背景
	for(y = 0; y < IMGH; y++)
	{
		*(g_lpDivide + y*IMGW) = 255;
		*(g_lpDivide + y*IMGW + IMGW - 1) = 255;
	}
	for(x = 0; x < IMGW; x++)
	{
		*(g_lpDivide + x) = 255;
		*(g_lpDivide + (IMGH-1)*IMGW + x) = 255;
	}

	for(y = 1; y < IMGH-1; y++)
	{
		for(x = 1; x < IMGW-1; x++)
		{
			// 根据幅值与阈值大小判断是否为背景区域
			if(*(g_lpTemp + y*IMGW + x) < threshold)
			{
				*(g_lpDivide + y*IMGW + x) = 255;
			}
			else
			{
				*(g_lpDivide + y*IMGW + x) = 0;
				num++;
			}
		}
	}

	// 如果前景区域面积小于总面积的十分之一，则表示前景区域太小，返回错误
	if(num < IMGSIZE/10)
		return 1;
	else
		return 0;

}

//
//	clearEdge: 清除背景
//
void clearEdge()
{
	sint32	x, y;
	sint32  temp;
	temp = 0;

	for(y = 0; y < IMGH; y++)
	{
		for(x = 0; x < IMGW; x++)
		{	
			// 如果是背景区域，则置该点为白点
			if(*(g_lpDivide + temp + x) == 255)
			{
				*(g_lpOrgFinger + temp + x) = 255;
				*(g_lpOrient + temp + x) = 255;
			}
		}
		temp += IMGW;
	}
}


#endif