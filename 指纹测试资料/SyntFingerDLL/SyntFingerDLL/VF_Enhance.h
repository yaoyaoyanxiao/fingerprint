#ifndef	__VFENHANCE_H__
#define __VFENHANCE_H__

#include "VF_Type.h"
#include "VF_Global.h"
#include "VF_Function.h"

//
//	orientFilter: 对指纹图像进行定向滤波增强
//
void orientFilter()
{
	sint32 x, y;
	sint32 i;
	sint32 d = 0;
	sint32 sum = 0;
	// 纹线方向上进行平滑滤波的平滑滤波器
	sint32 Hw[7] = {1, 1, 1, 1, 1, 1, 1};
	// 纹线方向的垂直方向上进行锐化滤波的锐化滤波器
	sint32 Vw[7] = {-3, -1, 3, 9, 3, -1, -3};
	sint32 hsum = 0;
	sint32 vsum = 0;
	sint32 temp = 0;
	uint8  *lpSrc = NULL;
	uint8  *lpDir = NULL;

	// 纹线方向上进行平滑滤波
	temp = 0;
	for(y = 0; y < IMGH; y++)
	{
		for(x = 0; x < IMGW; x++)
		{
			lpDir = g_lpOrient + temp + x;
			lpSrc = g_lpOrgFinger + temp + x;
			// 纹线方向的索引
			d = DIndex(*lpDir);
			sum = 0;
			hsum = 0;
			for(i = 0; i < 7; i++)
			{
				if(y+g_DSite[d][i][1] < 0 || y+g_DSite[d][i][1] >= IMGH ||
				   x+g_DSite[d][i][0] < 0 || x+g_DSite[d][i][0] >= IMGW)
				{
					continue;
				}
				sum += Hw[i]*(*(lpSrc + g_DSite[d][i][1]*IMGW + g_DSite[d][i][0]));
				hsum += Hw[i];
			}
			if(hsum != 0)
			{
				*(g_lpTemp + temp + x) = (uint8)(sum/hsum);
			}
			else
			{
				*(g_lpTemp + temp + x) = 255;
			}
		}
		temp += IMGW;
	}

	// 纹线方向的垂直方向上进行锐化滤波
	temp = 0;
	for(y = 0; y < IMGH; y++)
	{
		for(x = 0; x < IMGW; x++)
		{
			lpDir = g_lpOrient + temp + x;
			lpSrc = g_lpTemp + temp + x;
			
			// 纹线方向的垂直方向的索引
			d = (DIndex(*lpDir)+6) % 12;

			sum = 0;
			vsum = 0;
			for(i = 0; i < 7; i++)
			{
				if(y+g_DSite[d][i][1] < 0 || y+g_DSite[d][i][1] >= IMGH ||
				   x+g_DSite[d][i][0] < 0 || x+g_DSite[d][i][0] >= IMGW)
				{
					continue;
				}
				sum += Vw[i]*(*(lpSrc + g_DSite[d][i][1]*IMGW + g_DSite[d][i][0]));
				vsum += Vw[i];
			}
			if(vsum > 0)
			{
				sum /= vsum;
				if(sum > 255)
				{
					*(g_lpOrgFinger + temp + x) = 255;
				}
				else if(sum < 0)
				{
					*(g_lpOrgFinger + temp + x) = 0;
				}
				else
				{
					*(g_lpOrgFinger + temp + x) = (uint8)sum;
				}
			}
			else
			{
				*(g_lpOrgFinger + temp + x) = 255;
			}
		}
		temp += IMGW;
	}

}


#endif