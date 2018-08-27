#ifndef	__VFSMOOTH_H__
#define __VFSMOOTH_H__

#include "VF_Type.h"
#include "VF_Global.h"

//
//	smooth: 对图像进行均值平滑滤波,消除噪声
//
void smooth(uint8 *lpInBuffer, uint8 *lpOutBuffer, sint32 r, sint32 d)
{
////////////////////////////////////////////////////////////////////////
//	lpInBuffer: [in] 要平滑的图像数据缓冲区
//	lpOutBuffer: [out] 平滑后的图像数据缓冲区
//	r: [in] 平滑滤波器半径
//	d: [in] 平滑滤波器步长
////////////////////////////////////////////////////////////////////////
	sint32	x, y, i, j;
	uint8	*lpSrc;
	uint8	*lpRst;
	sint32	sum, num;
	
	if(d == 2)
	{
		for(y = 0; y < IMGH; y++)
		{
			for(x = 0; x < IMGW; x++)
			{
				lpSrc = lpInBuffer + y*IMGW + x;//原图像指针
				lpRst = lpOutBuffer + y*IMGW + x;//目标图像指针
				sum = 0;						//像素和总量
				num = 0;						//像素量
				for(i = -r; i <= r; i+=2)		//在中心为（x，y）边长为2r的矩形中搜索
				{
					if(i+y<0 || i+y>=IMGH)
						continue;
					for(j = -r; j <= r; j+=2)
					{
						if(j+x<0 || j+x>=IMGW)
							continue;
						sum += *(lpSrc + i*IMGW + j);//求像素和
						num++;						//求像素个数
					}
				}
				*lpRst = (uint8)(sum/num);			//求平均
			}
		}
	}
	else
	{
		for(y = 0; y < IMGH; y++)
		{
			for(x = 0; x < IMGW; x++)
			{
				lpSrc = lpInBuffer + y*IMGW + x;
				lpRst = lpOutBuffer + y*IMGW + x;
				sum = 0; 
				num = 0;
				for(i = -r; i <= r; i++)
				{
					if(i+y<0 || i+y>=IMGH)
						continue;
					for(j = -r; j <= r; j++)
					{
						if(j+x<0 || j+x>=IMGW)
							continue;
						sum += *(lpSrc + i*IMGW + j);
						num++;
					}
				}
				*lpRst = (uint8)(sum/num);
			}
		}
	}

}


#endif