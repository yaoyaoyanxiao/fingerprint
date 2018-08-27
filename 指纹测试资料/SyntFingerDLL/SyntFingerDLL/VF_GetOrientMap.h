#ifndef	__VFGETORIENTMAP_H__
#define __VFGETORIENTMAP_H__

#include "VF_Type.h"
#include "VF_Global.h"

//
//	getOrientMap: 求指纹方向场
//
void getOrientMap(sint32  r)
{
/////////////////////////////////////////////////////////////////////////
//	r: [in] 公式求算纹线方向的窗口半径，最好在(8 - 16)
/////////////////////////////////////////////////////////////////////////
	sint32	x, y, i, j;
	sint32  vx, vy, lvx, lvy;
	uint8   *lpSrc = NULL;
	uint8   *lpDiv = NULL;
	uint8   *lpOri = NULL;
	sint32	angle, grad, gradSum, num;
	double  fAngle;

	for(y = 0; y < IMGH/2; y++)
	{
		for(x = 0; x < IMGW/2; x++)
		{
			lpDiv = g_lpDivide + 2*y*IMGW + 2*x;
			lpOri = g_lpOrient + 2*y*IMGW + 2*x;
			lvx = 0;
			lvy = 0;
			num = 0;
			gradSum = 0;
			for(i = -r; i <= r; i++)	// 为提高速度，步长为2
			{
				if(y+i<1 || y+i>=IMGH/2-1)//:个人认为这里可修改,包括外面的for循环IMGH/2、IMGW/2
					continue;
				for(j = -r; j <= r; j++)	// 为提高速度，步长为2
				{
					if(x+j<1 || x+j>=IMGW/2-1)//:个人认为这里可修改
						continue;
					lpSrc = g_lpTemp + (y+i)*(IMGW/2) + x+j;

					vx = *(lpSrc + IMGW/2 + 1) - *(lpSrc + IMGW/2 - 1) +
						 *(lpSrc + 1)*2 - *(lpSrc - 1)*2 +
						 *(lpSrc - IMGW/2 + 1) - *(lpSrc - IMGW/2 - 1);

					vy = *(lpSrc + IMGW/2 - 1) - *(lpSrc - IMGW/2 - 1) +
						 *(lpSrc + IMGW/2)*2 - *(lpSrc - IMGW/2)*2 +
						 *(lpSrc + IMGW/2 + 1) - *(lpSrc - IMGW/2 + 1);

					gradSum += (abs(vx)+abs(vy));
					lvx += vx * vy * 2;
					lvy += vx*vx - vy*vy;
					num++;
				}
			}
			if(num == 0)
				num = 1;
			// 求幅值，保存到g_lpDivide中，用于分割前景背景
			grad = gradSum/num;
			
			if(grad > 255)
				grad = 255;
			*lpDiv = (uint8)grad;
			*(lpDiv + 1) = (uint8)grad;
			*(lpDiv + IMGW) = (uint8)grad;
			*(lpDiv + IMGW + 1) = (uint8)grad;

			// 求弧度
			fAngle = atan2(double(lvy), double(lvx));
			// 变换到(0 - 2*pi)
			if(fAngle < 0)
			{
				fAngle += 2*PI;
			}
			// 求纹线角度
			fAngle = (fAngle*EPI*0.5 + 0.5);
			angle = (sint32)fAngle;

			// 因为采用 sobel算子，所以角度偏转了135度，所以要旋转求得的角度
			angle -= 135;
			// 角度变换到（0-180）
			if(angle <= 0)
			{
				angle += 180;
			}
			angle = 180-angle;
			// 最终纹线角度
			*lpOri = (uint8)angle;
			*(lpOri + 1) = (uint8)angle;
			*(lpOri + IMGW) = (uint8)angle;
			*(lpOri + IMGW + 1) = (uint8)angle;

		}
	}
	
}

#endif
