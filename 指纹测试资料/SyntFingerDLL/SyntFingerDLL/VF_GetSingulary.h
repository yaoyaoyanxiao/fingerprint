#ifndef	__VFGETSINGULAR_H__
#define __VFGETSINGULAR_H__

#include "VF_Type.h"
#include "VF_Global.h"
#include "VF_Function.h"
#include "StdAfx.h"

int getOriChange(int angle1, int angle2, char flag)
{
	int  d;

	d = angle2 - angle1;
	if(flag >= 1)
	{
		if(d < 0)
		{
			d += 10;
		}

	}
	else if(flag <= -1)
	{
		if(d > 0)
		{
			d -= 10;
		}
	}
	
	return d;
}


int getSingular(BYTE *lpOrient, int Width, int Height, char flag)
{
	int    x, y, i;
	int    num;
	int    sum1, sum2;
	int    d;
	unsigned char *pOriMap;

	int    oriV;
	int    a1, a2;
	DBLPOINT  singuArr[30];

	int    value;
	double dis;
	bool   bFound = false;
	bool   fg = false;
	int    D3[8][2] = {
		-1,-1, -1,0,  -1,1,  0,1,  1,1,  1,0,  1,-1,  0,-1
	};
	int    D5[12][2] = {
		-2,-1, -2,0,  -2,1, -1,2,  0,2,  1,2, 2,1,  2,0,  2,-1, 1,-2, 0,-2, -1,-2
	};
	int    D7[24][2] = {
		-3,-3, -3,-2, -3,-1, -3,0, -3,1, -3,2, -3,3, -2,3, -1,3, 0,3, 1,3, 2,3, 3,3, 3,2, 3,1, 3,0,
			3,-1, 3,-2, 3,-3, 2,-3, 1,-3, 0,-3, -1,-3, -2,-3
	};
	

	num = 0;

	memset(singuArr, 0, sizeof(singuArr));				//初始化奇异点数组，清零

	for(y = 3; y < Height-3; y++)
	{
		for(x = 3; x < Width-3; x++)					//遍历整幅图
		{
			pOriMap = lpOrient + y*Width + x;			//获得某点的方向场指针
			oriV = *pOriMap;							//获取某点的方向场的值

			if(oriV == 255)								//若是背景区域，则跳入下一个循环
			{
				continue;
			}

			fg = false;									
			for(i = 0; i < 24; i++)
			{
				if(pOriMap[D7[i][1]*Width + D7[i][0]]==255)
				{
					fg = true;
					break;
				}
			}
			if(fg)
			{
				continue;
			}

			sum1 = 0;
			for(i = 0; i < 8; i++)
			{
				a1 = pOriMap[D3[i][1]*Width + D3[i][0]]/24;
				a2 = pOriMap[D3[(i+1)%8][1]*Width + D3[(i+1)%8][0]]/24;
				d = getOriChange(a1, a2, flag);
				
				if(abs(d) > 5)
				{
					break;
				}
				
				sum1 += d;
			}
			sum2 = 0;
			for(i = 0; i < 12; i++)
			{
				a1 = pOriMap[D5[i][1]*Width + D5[i][0]]/24;
				a2 = pOriMap[D5[(i+1)%12][1]*Width + D5[(i+1)%12][0]]/24;
				d = getOriChange(a1, a2, flag);
				
				if(abs(d) > 5)
				{
					break;
				}
				
				sum2 += d;
			}
		    
			if(flag == -1)									//中心点
			{
				value = -10;
			}
			else if(flag == 1)								//三角点
			{
				value = 10;
			}
			if(sum2 == value && sum1 == value)
			{
				bFound = false;
				for(i = 0; i < num; i++)
				{
					dis = sqrt((double)((x - singuArr[i].x)*(x - singuArr[i].x) +
						(y - singuArr[i].y)*(y - singuArr[i].y)));
					if(dis < 4)
					{
						singuArr[i].x = (singuArr[i].x + x)/2.0;
						singuArr[i].y = (singuArr[i].y + y)/2.0;
						bFound = true;
						break;
					}

				}
				if(!bFound)
				{
					singuArr[num].x = x;
					singuArr[num].y = y;
					num++;
					if(num >= MAX_SINGULARYNUM)				//若奇异点数量超过最大限制，则停止寻找奇异点
					{
						goto next;
					}
				}

			}
		}
	}

next:

	int  trilen = 16;
	int N = 0;											//奇异点的个数
	for(i = 0; i < num; i++)								//将找到的奇异点存入输入数组
	{
		//lpArr[*lpNum + i].x = (long)singuArr[i].x;
		//lpArr[*lpNum + i].y = (long)singuArr[i].y;	

		int x = (long)singuArr[i].x;
		int y = (long)singuArr[i].y;
		
		if (*(g_lpDivide + y * Width + x) == 255) 
			continue;

		if (((x + 0.0) / Width < 0.1) || ((x + 0.0) / Width > 0.9)) 
			continue;

		if (((y + 0.0) / Height < 0.1 ) || ((y + 0.0) / Height > 0.9))
			continue;

		

		g_Feature.MinutiaArr[g_Feature.MinutiaNum + N].x = x;
		g_Feature.MinutiaArr[g_Feature.MinutiaNum + N].y = y;

		g_Feature.MinutiaArr[g_Feature.MinutiaNum + N].Type = (flag == -1) ? VF_MINUTIA_CORE : VF_MINUTIA_DELTA;				
		g_Feature.MinutiaArr[g_Feature.MinutiaNum + N].Direction = 0;				

		N++;
	}

	g_Feature.MinutiaNum += N;

	return 0;
}




#endif