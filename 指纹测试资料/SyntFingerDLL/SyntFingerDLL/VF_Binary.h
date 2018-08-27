#ifndef	__VFBINARY_H__
#define __VFBINARY_H__

#include "VF_Type.h"
#include "VF_Global.h"
#include "VF_Function.h"

//
//	binary: 对指纹图像进行二值化
//
sint32 binary()
{
	sint32 x, y;
	sint32 i;
	sint32 d = 0;
	sint32 sum = 0;
	// 纹线方向上的7个点的权值
	sint32 Hw[7] = {2, 2, 3, 4, 3, 2, 2};
	// 纹线方向的垂直方向上的7个点的权值
	sint32 Vw[7] = {1, 1, 1, 1, 1, 1, 1};
	sint32 hsum = 0;	// 纹线方向上的7个点的加权和
	sint32 vsum = 0;	// 纹线方向的垂直方向上的7个点的加权和
	sint32 Hv = 0;		// 纹线方向上的7个点的加权平均值
	sint32 Vv = 0;		// 纹线方向的垂直方向上的7个点的加权平均值
	sint32 temp = 0;
	uint8  *lpSrc = NULL;	// 指纹图像像素点指针
	uint8  *lpDir = NULL;	// 纹线方向指针

	temp = 0;
	for(y = 0; y < IMGH; y++)
	{
		for(x = 0; x < IMGW; x++)
		{
			lpDir = g_lpOrient + temp + x;
			lpSrc = g_lpOrgFinger + temp + x;
			
			// 如果该点非常黑，则在临时缓冲区内置该点为黑点，值为0
			if(*lpSrc < 4)
			{
				*(g_lpTemp + temp + x) = 0;
				continue;
			}
			// 计算方向索引（量化为12个方向）
			d = DIndex(*lpDir);
			
			// 计算当前点在纹线方向上的加权平均值
			sum = 0;
			hsum = 0;
			for(i = 0; i < 7; i++)
			{
				// 坐标是否越界
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
				Hv = sum/hsum;
			}
			else
			{
				Hv = 255;
			}

			// 纹线方向的垂直方向的索引
			d = (d+6)%12;

			// 计算当前点在纹线方向的垂直方向上的加权平均值
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
			if(vsum != 0)
			{
				Vv = sum/vsum;
			}
			else
			{
				Vv = 255;
			}

			
			if(Hv < Vv)
			{
				// 纹线方向上加权平均值较小则置当前点为黑点
				*(g_lpTemp + temp + x) = 0;
			}
			else
			{
				// 纹线方向上加权平均值较大则置当前点为白点
				*(g_lpTemp + temp + x) = 255;
			}
		}
		temp += IMGW;
	}

	// 将临时缓冲区内数据拷贝到原始图像数据缓冲区
	memcpy((void *)g_lpOrgFinger, (void *)g_lpTemp, IMGSIZE);
	

	return 0;

}

//
//	binary: 对二值化指纹图像进行去噪声
//
void binaryClear()
{
	sint32 x, y, i;
	sint32 num = 0;
	bool   bWorking;
	sint32 temp;
	uint8  *lpSrc = NULL;
	bWorking = true;
	sint32  n = 0;
	// 某点周围8个点的地址偏移
	sint32	SiteD8[8] = {IMGW-1, IMGW, IMGW+1, 1, -IMGW+1, -IMGW, -IMGW-1, -1};
	
	// 初始化临时缓冲区
	memset((void *)g_lpTemp, 255, IMGSIZE);

	// 循环处理直到处理完毕或者处理次数超过8
	while(bWorking && n < 8)
	{
		bWorking = false;
		n++;
		temp = IMGW;
		for(y = 1; y < IMGH-1; y++)
		{
			for(x = 1; x < IMGW-1; x++)
			{
				// 背景的点不处理
				if(*(g_lpDivide + temp + x) == 255)
				{
					continue;
				}
				// 统计当前点周围与它相同类型点的个数
				num = 0;
				lpSrc = g_lpOrgFinger + temp + x;
				for(i = 0; i < 8; i++)
				{
					if(*(lpSrc+SiteD8[i]) == *lpSrc)
					{
						num++;
					}
				}
				// 相同点个数小于二则改变当前点类型
				if(num < 2)
				{
					*(g_lpTemp+temp+x) = 255 - *lpSrc;
					bWorking = true;
				}
				else
				{
					*(g_lpTemp+temp+x) = *lpSrc;
				}

			}
			temp += IMGW;
		}
		// 将处理结果拷贝到原始图像缓冲区
		memcpy((void *)g_lpOrgFinger, (void *)g_lpTemp, IMGSIZE);
	}

}


#endif
