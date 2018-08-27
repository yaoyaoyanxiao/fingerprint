#ifndef	__VFGETMINUTIA_H__
#define __VFGETMINUTIA_H__

#include "VF_Type.h"
#include "VF_Global.h"
#include "VF_Function.h"

//
//	getMinutia: 提取指纹特征
//
sint32  getMinutia()
{
	sint32	x, y, i, j, k, n;
	sint32  temp;
	sint32  sum;
	uint8   *lpNow;
	uint8   *lpDis;
	sint32  r = 16;		// 允许特征点靠近前景边缘的最近距离
	double  d, d1, d2, d0, a;
	sint32  dGate = 16;
	bool    flag;
	sint32  x11, y11;
	sint32  x21, y21;
	sint32  angle1, angle2, angle3, angle4;
	uint8   *tempPtr[8];
	uint8   *disPtr[3];
	uint8   *lpOri;
	sint32  tempForkNum;
	sint32  tempEndNum;
	sint32  ForkNum;
	sint32  EndNum;
	// 临时特征点数组
	MINUTIA     tempFork[MAXRAWMINUTIANUM];
	MINUTIA     tempEnd[MAXRAWMINUTIANUM];
	MINUTIA     ForkArr[MAXRAWMINUTIANUM];
	MINUTIA     EndArr[MAXRAWMINUTIANUM];
	// 某点为圆心，半径为5的圆上所有点的地址偏移
	sint32	SiteR5[28] = {
		-5, IMGW-5, 2*IMGW-5, 3*IMGW-4, 4*IMGW-3, 5*IMGW-2, 5*IMGW-1, 5*IMGW,
		5*IMGW+1, 5*IMGW+2, 4*IMGW+3, 3*IMGW+4, 2*IMGW+5, IMGW+5, 5, -IMGW+5,
		-2*IMGW+5, -3*IMGW+4, -4*IMGW+3, -5*IMGW+2, -5*IMGW+1, -5*IMGW,
		-5*IMGW-1, -5*IMGW-2, -4*IMGW-3, -3*IMGW-4, -2*IMGW-5, -IMGW-5
	}; 
	// 某点周围8个点的地址偏移
	sint32	SiteU8[8] = {IMGW-1, IMGW, IMGW+1, 1, -IMGW+1, -IMGW, -IMGW-1, -1};

	// 初始化临时特征点数组
	memset((void *)&tempFork[0], 0, MAXRAWMINUTIANUM*sizeof(MINUTIA));
	memset((void *)&tempEnd[0], 0, MAXRAWMINUTIANUM*sizeof(MINUTIA));
	memset((void *)&ForkArr[0], 0, MAXRAWMINUTIANUM*sizeof(MINUTIA));
	memset((void *)&EndArr[0], 0, MAXRAWMINUTIANUM*sizeof(MINUTIA));


	bool    bGood = false;	// 标记阈值是否调整好，使特征点数目在允许范围内
	sint32  densD = 7;	// 允许两个特征点间最近的距离
	sint32  loopnum;	// 循环次数
	sint32  trilen = 16;
	// 第一遍寻找所有端点和叉点	
	ForkNum = 0; 
	EndNum = 0; 
	temp = 17 * IMGW;
	for(y = 17; y < IMGH-17; y++)
	{
		for(x = 17; x < IMGW-17; x++)
		{
			lpNow = g_lpOrgFinger + temp + x;//原图像指针
			lpOri = g_lpOrient + y*IMGW + x;//原图像方向场指针
			// 不是黑点则不考虑
			if(*lpNow != 0)
			{
				continue;
			}
			// 是叉点
			if(IsFork(lpNow))
			{
				// 检查是否靠近边缘, 靠近边缘则不考虑
				flag = true;
				for(i = -r; i <= r && flag; i++)
				{
					for(j = -r; j <= r && flag; j++)
					{
						if(y+i<0 || y+i>=IMGH || x+j<0 || x+j>=IMGW)
						{
							continue;
						}

						if(*(g_lpOrient + temp + i*IMGW + x + j) == 255)
						{
							flag = false;
							break;
						}
					}
				}
				// 检查该点周围方向场是否变化剧烈，变化剧烈则不考虑，因为很可能是虚假点
				sum = 0;
				for(i = 0; i < 28; i++)
				{
					sum += GetJiajiao(*(lpOri+SiteR5[(i+1)%28]), *(lpOri+SiteR5[i]));//相邻两个方向的夹角
				}
				if(sum > 96)
					flag = false;
				
				// 达到所有要求则记录下来该点
				if(flag)
				{
					ForkArr[ForkNum].x = x;
					ForkArr[ForkNum].y = y;
					ForkNum++;
					// 如果总数已经超过允许最大数目，则返回错误
					if(ForkNum >= MAXRAWMINUTIANUM)
					{
						ForkNum = 0;
						return 1;
					}

				}

			}
			else if(IsEnd(lpNow))	// 如果是端点
			{
				// 检查是否靠近边缘, 靠近边缘则不考虑
				flag = true;
				for(i = -r; i <= r && flag; i++)
				{
					for(j = -r; j <= r && flag; j++)
					{
						if(y+i<0 || y+i>=IMGH || x+j<0 || x+j>=IMGW)
						{
							continue;
						}

						if(*(g_lpOrient + temp + i*IMGW + x + j) == 255)
						{
							flag = false;
							break;
						}
					}
				}

				// 检查该点周围方向场是否变化剧烈，变化剧烈则不考虑，因为很可能是虚假点
				sum = 0;
				for(i = 0; i < 28; i++)
				{
					sum += GetJiajiao(*(lpOri+SiteR5[(i+1)%28]), *(lpOri+SiteR5[i]));
				}
				if(sum > 96)
					flag = false;

				// 达到所有要求则记录下来该点
				if(flag)
				{
					EndArr[EndNum].x = x;
					EndArr[EndNum].y = y;
					EndNum++;
					// 如果总数已经超过允许最大数目，则返回错误
					if(EndNum >= MAXRAWMINUTIANUM)
					{
						EndNum = 0;
						return 1;
					}

				}

			}
		}
		temp += IMGW;
	}

	// 初始化临时缓冲区，准备循环调整两个特征点间允许的最小距离，使之增大到一个合适的值
	// 因为如果特征点非常密集的地方很有可能是质量很差的地方
	for(i = 0; i < MAXRAWMINUTIANUM; i++)
	{
		tempEnd[i] = EndArr[i];
	}
	for(i = 0; i < MAXRAWMINUTIANUM; i++)
	{
		tempFork[i] = ForkArr[i];
	}
	tempForkNum = ForkNum;
	tempEndNum = EndNum;

	// 循环调整
	bGood = false;
	loopnum = 0;	// 调整的次数
	while(!bGood && loopnum < 32) // 最多调整32次
	{
		loopnum++;	// 调整次数加一
		// 得到新的特征点数组
		for(i = 0; i < MAXRAWMINUTIANUM; i++)
		{
			EndArr[i] = tempEnd[i];
		}
		for(i = 0; i < MAXRAWMINUTIANUM; i++)
		{
			ForkArr[i] = tempFork[i];
		}
		// 新的特征点数
		ForkNum = tempForkNum;	// 叉点
		EndNum = tempEndNum;	// 端点

		// 去掉虚假的端点
		bGood = true;
		for(i = 0; i < EndNum-1; i++)
		{
			flag = false;
			for(j = i+1; j < EndNum; j++)
			{
				// 求两个端点的距离
				d = sqrt((double)((EndArr[i].x-EndArr[j].x)*(EndArr[i].x-EndArr[j].x) +
					(EndArr[i].y-EndArr[j].y)*(EndArr[i].y-EndArr[j].y)));
				// 距离足够大则检查下一个端点
				if(d > dGate && d > densD)
				{
					continue;
				}
				// 距离太小，则将其坐标置为（0，0）
				if(d <= densD)
				{
					EndArr[j].x = 0;
					EndArr[j].y = 0;
					flag = true;
					continue;
				}
				// 求第一个端点所在纹线的角度
				lpNow = g_lpOrgFinger + EndArr[i].y*IMGW + EndArr[i].x;
				if(GetByDis(lpNow, &lpDis, 8) != 0)
				{
					break;
				}
				angle1 = GetAngle(EndArr[i].x, EndArr[i].y, PX(lpDis), PY(lpDis));
				
				// 求第二个端点所在纹线的角度
				lpNow = g_lpOrgFinger + EndArr[j].y*IMGW + EndArr[j].x;
				if(GetByDis(lpNow, &lpDis, 8) != 0)
				{
					continue;
				}
				angle2 = GetAngle(EndArr[j].x, EndArr[j].y, PX(lpDis), PY(lpDis));

				// 求两个角度间的距离
				angle3 = GetAngleDis(angle1, angle2);
				
				// 如果两个角度间成锐角，则不是虚假特征点
				if(angle3 > 270 || angle3 < 90)
				{
					continue;
				}

				// 求两个端点连线的角度
				angle3 = GetAngle(EndArr[i].x, EndArr[i].y, EndArr[j].x, EndArr[j].y);
				// 求第一个端点纹线与连线的夹角
				angle3 = GetAngleDis(angle1, angle3);
				// 如果夹角较大则不是虚假点
				if(angle3 < 150 || angle3 > 210)
				{
					continue;
				}

				// 求第二个端点纹线与连线的夹角
				angle4 = GetAngle(EndArr[j].x, EndArr[j].y, EndArr[i].x, EndArr[i].y);
				angle4 = GetAngleDis(angle2, angle4);
				
				// 如果夹角较大则不是虚假点
				if(angle4 < 150 || angle4 > 210)
				{
					continue;
				}
				// 否则表示这两个点是同一条纹线上的断裂处的两个端点,坐标置原点
				EndArr[j].x = 0;
				EndArr[j].y = 0;
				flag = true;
			}
			if(flag)// 表示这两个点是同一条纹线上的断裂处的两个端点,坐标置原点
			{
				EndArr[i].x = 0;
				EndArr[i].y = 0;
			}
		}

		// 统计新的端点数目
		j = 0;
		for(i = 0; i < EndNum; i++)
		{
			if(EndArr[i].x == 0 || EndArr[i].y == 0)
			{
				continue;
			}
			lpNow = g_lpOrgFinger + EndArr[i].y*IMGW + EndArr[i].x;
			if(GetByDis(lpNow, &lpDis, 8) != 0)
			{
				continue;
			}

			j++;
			// 如果端点数目还是过多，则需要调整densD
			if(j >= MAXMINUTIANUM)
			{
				bGood = false;
				break;
			}

		}
		// 调整densD
		if(!bGood)
		{
			densD++; // 加一
			continue;
		}

		// 将端点装配到g_Feature
		j = 0;
		for(i = 0; i < EndNum; i++)
		{
			if(EndArr[i].x == 0 || EndArr[i].y == 0)
			{
				continue;
			}
			// 沿纹线找到第8个点
			lpNow = g_lpOrgFinger + EndArr[i].y * IMGW + EndArr[i].x;
			if(GetByDis(lpNow, &lpDis, 8) != 0)
			{
				continue;
			}
			// 求该点与端点连线的角度
			angle1 = GetAngle(EndArr[i].x, EndArr[i].y, PX(lpDis), PY(lpDis));
			angle2 = *(g_lpOrient + EndArr[i].y*IMGW + EndArr[i].x);
			angle3 = GetAngleDis(angle1, angle2);
			// 如果成钝角，则表示该特征点方向与该点方向场方向相反
			if(angle3 > 90 && angle3 < 270)
			{
				g_Feature.MinutiaArr[j].Direction = angle2 + 180;
			}
			else // 如果成锐角，则表示该特征点方向与该点方向场方向相同
			{
				g_Feature.MinutiaArr[j].Direction = angle2;
			}

			//若两点互相匹配，则两点周围的相对方向场相似
			a = g_Feature.MinutiaArr[j].Direction / EPI + PI/4.0;
			x11 = EndArr[i].x + (sint32)(trilen*cos(a)+ 0.5);
			y11 = EndArr[i].y + (sint32)(trilen*sin(a)+ 0.5);
			g_Feature.MinutiaArr[j].Triangle[0] = *(g_lpOrient + y11*IMGW + x11);
			a += PI*2/3.0;
			x11 = EndArr[i].x + (sint32)(trilen*cos(a)+ 0.5);
			y11 = EndArr[i].y + (sint32)(trilen*sin(a)+ 0.5);
			g_Feature.MinutiaArr[j].Triangle[1] = *(g_lpOrient + y11*IMGW + x11);
			a += PI*2/3.0;
			x11 = EndArr[i].x + (sint32)(trilen*cos(a)+ 0.5);
			y11 = EndArr[i].y + (sint32)(trilen*sin(a)+ 0.5);
			g_Feature.MinutiaArr[j].Triangle[2] = *(g_lpOrient + y11*IMGW + x11);

			g_Feature.MinutiaArr[j].x = EndArr[i].x;	//横坐标
			g_Feature.MinutiaArr[j].y = EndArr[i].y;	//纵坐标
			g_Feature.MinutiaArr[j].Type = VF_MINUTIA_END;	//类型

			j++;

		}

		EndNum = j;	// 端点数目


		// 去掉距离太近的叉点
		for(i = 0; i < ForkNum-1; i++)
		{
			flag = false;
			for(j = i+1; j < ForkNum; j++)
			{
				d = sqrt((double)((ForkArr[i].x-ForkArr[j].x)*(ForkArr[i].x-ForkArr[j].x) +
					(ForkArr[i].y-ForkArr[j].y)*(ForkArr[i].y-ForkArr[j].y)));

				if(d <= densD-2)
				{
					ForkArr[j].x = 0;
					ForkArr[j].y = 0;
					flag = true;
				}
			}
			if(flag)
			{
				ForkArr[i].x = 0;
				ForkArr[i].y = 0;
			}
		}

		// 统计新的真正的叉点
		j = 0;
		for(i = 0; i < ForkNum; i++)
		{
			if(ForkArr[i].x == 0 || ForkArr[i].y == 0)
			{
				continue;
			}
			lpNow = g_lpOrgFinger + ForkArr[i].y*IMGW + ForkArr[i].x;

			tempPtr[0] = lpNow;
			*tempPtr[0] = 255;
			// 检查是否真正叉点
			n = 0;
			for(k = 0; k < 8; k++)
			{
				if(*(lpNow + SiteU8[k]) == 0)
				{
					tempPtr[n+1] = lpNow + SiteU8[k];
					*tempPtr[n+1] = 255;
					n++;
				}
			}
			for(k = 0; k < 4; k++)
			{
				*tempPtr[k] = 0;
			}

			if(n != 3)
			{
				continue;
			}
			j++;
			// 特征点总数大于允许最大数目则要调整densD
			if(EndNum + j >= MAXMINUTIANUM)
			{
				densD++;
				bGood = false;
				break;
			}

		}

		if(!bGood)
		{
			continue;
		}

		// 装配叉点到g_Feature
		j = 0;
		for(i = 0; i < ForkNum; i++)
		{
			if(ForkArr[i].x == 0 || ForkArr[i].y == 0)
			{
				continue;
			}
			lpNow = g_lpOrgFinger + ForkArr[i].y*IMGW + ForkArr[i].x;

			tempPtr[0] = lpNow;
			*tempPtr[0] = 255;
			n = 0;
			for(k = 0; k < 8; k++)
			{
				if(*(lpNow + SiteU8[k]) == 0)
				{
					tempPtr[n+1] = lpNow + SiteU8[k];
					*tempPtr[n+1] = 255;
					n++;
				}
			}
			if(n != 3)
			{
				continue;
			}
			// 找到叉点三个方向纹线上距离为8的点
			for(k = 0; k < 3; k++)
			{
				GetByDis(tempPtr[k+1], &disPtr[k], 8);
			}
			for(k = 0; k < 4; k++)
			{
				*tempPtr[k] = 0;
			}
			// 求该三个点两两间的距离
			d0 = sqrt((double)((PX(disPtr[1]) - PX(disPtr[2]))*(PX(disPtr[1]) - PX(disPtr[2])) +
				(PY(disPtr[1]) - PY(disPtr[2]))*(PY(disPtr[1]) - PY(disPtr[2]))));
			d1 = sqrt((double)((PX(disPtr[0]) - PX(disPtr[2]))*(PX(disPtr[0]) - PX(disPtr[2])) +
				(PY(disPtr[0]) - PY(disPtr[2]))*(PY(disPtr[0]) - PY(disPtr[2]))));
			d2 = sqrt((double)((PX(disPtr[1]) - PX(disPtr[0]))*(PX(disPtr[1]) - PX(disPtr[0])) +
				(PY(disPtr[1]) - PY(disPtr[0]))*(PY(disPtr[1]) - PY(disPtr[0]))));

			// 距离最短的边对应的是叉线方向
			if(d0 < d1 && d0 < d2)
			{
				x11 = PX(disPtr[1]);
				y11 = PY(disPtr[1]);
				x21 = PX(disPtr[2]);
				y21 = PY(disPtr[2]);
				lpDis = disPtr[0];
			}
			else if(d1 < d0 && d1 < d2)
			{
				x11 = PX(disPtr[0]);
				y11 = PY(disPtr[0]);
				x21 = PX(disPtr[2]);
				y21 = PY(disPtr[2]);
				lpDis = disPtr[1];
			}
			else
			{
				x11 = PX(disPtr[0]);
				y11 = PY(disPtr[0]);
				x21 = PX(disPtr[1]);
				y21 = PY(disPtr[1]);
				lpDis = disPtr[2];
			}
			// 求最短边中点与叉点连线的方向与该点方向场方向的夹角
			angle1 = GetAngle(ForkArr[i].x, ForkArr[i].y, (x11+x21)/2, (y11+y21)/2);
			angle2 = *(g_lpOrient + ForkArr[i].y*IMGW + ForkArr[i].x);
			angle3 = GetAngleDis(angle1, angle2);
			// 如果成钝角，则表示该特征点方向与该点方向场方向相反
			if(angle3 > 90 && angle3 < 270)
			{
				g_Feature.MinutiaArr[EndNum + j].Direction = angle2 + 180;
			}
			else // 如果成锐角，则表示该特征点方向与该点方向场方向相同
			{
				g_Feature.MinutiaArr[EndNum + j].Direction = angle2;
			}
			// 纪录以特征点为中心外接圆半径为trilen的正三角形三个顶点的方向
			a = g_Feature.MinutiaArr[EndNum + j].Direction / EPI + PI/4.0;
			x11 = EndArr[i].x + (sint32)(trilen*cos(a)+ 0.5);
			y11 = EndArr[i].y + (sint32)(trilen*sin(a)+ 0.5);
			g_Feature.MinutiaArr[EndNum + j].Triangle[0] = *(g_lpOrient + y11*IMGW + x11);
			a += PI*2/3.0;
			x11 = EndArr[i].x + (sint32)(trilen*cos(a)+ 0.5);
			y11 = EndArr[i].y + (sint32)(trilen*sin(a)+ 0.5);
			g_Feature.MinutiaArr[EndNum + j].Triangle[1] = *(g_lpOrient + y11*IMGW + x11);
			a += PI*2/3.0;
			x11 = EndArr[i].x + (sint32)(trilen*cos(a)+ 0.5);
			y11 = EndArr[i].y + (sint32)(trilen*sin(a)+ 0.5);
			g_Feature.MinutiaArr[EndNum + j].Triangle[2] = *(g_lpOrient + y11*IMGW + x11);

			g_Feature.MinutiaArr[EndNum + j].x = ForkArr[i].x; //横坐标
			g_Feature.MinutiaArr[EndNum + j].y = ForkArr[i].y; //纵坐标
			g_Feature.MinutiaArr[EndNum + j].Type = VF_MINUTIA_FORK; // 类型


			j++;
		}
		ForkNum = j; // 叉点数目


	}

	// 如果循环32次都不能得到理想结果，则表示图像质量差，返回错误
	if(loopnum >= 32)
	{
		EndNum = 0;
		ForkNum = 0;
		return 1;
	}
	// 特征点数目等于端点数加叉点数
	g_Feature.MinutiaNum = (uint8)(EndNum+ForkNum);

	//  特征点数目太少则返回错误
	if(ForkNum + EndNum < 8)
	{
		EndNum = 0;
		ForkNum = 0;
		return 1;
	}

	return 0;
}

/*

//
//	getSingular： 提取中心点三角点
//
void  getSingular()
{
	sint32  x, y, i, j;
	sint32  DSite8[8][8];
	sint32  DSiteR1[8] = {-1, BOW-1, BOW, BOW+1, 1, -BOW+1, -BOW, -BOW-1};
	bool    flag, flag2, flag3, fg;
	sint32  temp;
	uint8   *lpDir;
	uint8	*lpCore;
	sint32  asum;
	sint32  mina;
	sint32  minindex;
	sint32  DAngleSum[8];
	sint32  ad;
	double  dis;
	VF_MINUTIAPTR    core;
	VF_MINUTIAPTR    delta;
	sint32	CoreNum, DeltaNum;
	//  半径为15的圆上100个点相对于圆心的地址偏移
	sint32 SiteR15[100] = {
		0*IMGW-15, 1*IMGW-15, 2*IMGW-15, 3*IMGW-15, 4*IMGW-15, 4*IMGW-14, 5*IMGW-14, 
		6*IMGW-14, 7*IMGW-13, 8*IMGW-13, 8*IMGW-12, 9*IMGW-12, 10*IMGW-11, 11*IMGW-11, 
		11*IMGW-10, 12*IMGW-9, 12*IMGW-8, 13*IMGW-8, 13*IMGW-7, 14*IMGW-6, 14*IMGW-5, 
		14*IMGW-4, 15*IMGW-4, 15*IMGW-3, 15*IMGW-2, 15*IMGW-1, 15*IMGW+0, 15*IMGW+1, 
		15*IMGW+2, 15*IMGW+3, 14*IMGW+3, 14*IMGW+4, 14*IMGW+5, 13*IMGW+6, 13*IMGW+7, 
		12*IMGW+7, 12*IMGW+8, 11*IMGW+9, 11*IMGW+10, 10*IMGW+10, 9*IMGW+11, 8*IMGW+11, 
		8*IMGW+12, 7*IMGW+12, 6*IMGW+13, 5*IMGW+13, 4*IMGW+13, 4*IMGW+14, 3*IMGW+14, 
		2*IMGW+14, 1*IMGW+14, 0*IMGW+14, -1*IMGW+14, -2*IMGW+14, -3*IMGW+14, -3*IMGW+13, 
		-4*IMGW+13, -5*IMGW+13, -6*IMGW+12, -7*IMGW+12, -7*IMGW+11, -8*IMGW+11, -9*IMGW+10, 
		-10*IMGW+10, -10*IMGW+9, -11*IMGW+8, -11*IMGW+7, -12*IMGW+7, -12*IMGW+6, -13*IMGW+5, 
		-13*IMGW+4, -13*IMGW+3, -14*IMGW+3, -14*IMGW+2, -14*IMGW+1, -14*IMGW+0, -14*IMGW-1, 
		-14*IMGW-2, -14*IMGW-3, -14*IMGW-4, -13*IMGW-4, -13*IMGW-5, -13*IMGW-6, -12*IMGW-7, 
		-12*IMGW-8, -11*IMGW-8, -11*IMGW-9, -10*IMGW-10, -10*IMGW-11, -9*IMGW-11, -8*IMGW-12, 
		-7*IMGW-12, -7*IMGW-13, -6*IMGW-13, -5*IMGW-14, -4*IMGW-14, -3*IMGW-14, -3*IMGW-15, 
		-2*IMGW-15, -1*IMGW-15
	};
	// 圆上100个点与圆心连线的角度（圆与水平线相交左边的交点为起始点）
	sint32    OriR15[100] = {
		180, 176, 172, 168, 165, 164, 160, 156, 151, 148, 146, 143, 137, 135, 
		132, 126, 123, 121, 118, 113, 109, 105, 104, 101, 97, 93, 90, 86, 
		82, 78, 77, 74, 70, 65, 61, 59, 56, 50, 47, 45, 39, 36, 
		33, 30, 24, 21, 17, 15, 12, 8, 4, 0, 355, 351, 347, 347, 
		342, 338, 333, 329, 327, 323, 318, 314, 311, 306, 302, 300, 296, 291, 
		287, 282, 282, 278, 274, 269, 265, 261, 257, 254, 252, 248, 245, 239, 
		236, 233, 230, 224, 222, 219, 213, 210, 208, 204, 199, 195, 192, 191, 
		187, 183
	};

	core = (VF_MINUTIAPTR)(g_lpMemory + sizeof(VF_FEATURE));
	delta = (VF_MINUTIAPTR)(g_lpMemory + sizeof(VF_FEATURE) + 
								sizeof(VF_MINUTIA)*5);

  // 量化的8个方向上的8个点的地址偏移
	for(i = 0; i < 8; i++)
	{
		DSite8[0][i] = i+1;
		DSite8[1][i] = (i+1)*(IMGW + 1);
		DSite8[2][i] = (i+1)*(IMGW);
		DSite8[3][i] = (i+1)*(IMGW - 1);
		DSite8[4][i] = -(i+1);
		DSite8[5][i] = -(i+1)*(IMGW + 1);
		DSite8[6][i] = -(i+1)*(IMGW);
		DSite8[7][i] = (i+1)*(-IMGW + 1);
	}

	CoreNum = 0;
	DeltaNum = 0;
	
	flag2 = true;
	flag3 = true;

	temp = VF_BW*IMGW;
	for(y = VF_BW; y <= IMGH-VF_BW; y++)
	{
		for(x = VF_BW; x <= IMGW-VF_BW; x++)
		{
			// 检查是否为边缘部分
			lpDir = g_lpBlockOrient + (y/VF_BW)*BOW + x/VF_BW;
			if(*lpDir == 255)
			{
				continue;
			}
			flag = true;
			for(i = 0; i < 8; i++)
			{
				if(*(lpDir + DSiteR1[i]) == 255)
				{
					flag = false;
					break;
				}				
			}
			if(!flag)
			{
				continue;
			}

			// 检查是否该点方向变化剧烈，变化剧烈才有可能是奇异点
			asum = 0;
			for(i = 0; i < 8; i++)
			{
				ad = GetJiajiao((sint32)(*(lpDir+DSiteR1[(i+1)%8])),
								(sint32)(*(lpDir+DSiteR1[i])));
				asum += ad;
			}
			if(abs(asum - 180) > 60)
			{
				continue;
			}
			
			// 检查8八个量化方向上的8个点是否为无效区域内
			lpDir = g_lpOrient + temp + x;
			if(*lpDir == 255)
			{
				continue;
			}
			flag = true;
			for(i = 0; i < 8 && flag; i++)
			{
				for(j = 0; j < 8 && flag; j++)
				{
					if(*(lpDir + DSite8[i][j]) == 255)
					{
						flag = false;
						break;
					}
				}
			}
			if(!flag)
			{
				continue;
			}
			// 检查第二圈上的夹角和是否比较大，如果是奇异点这个值应该较大
			asum = 0;
			for(i = 0; i < 8; i++)
			{
				asum += GetJiajiao((sint32)(*(lpDir+DSite8[(i+1)%8][2])),
					(sint32)(*(lpDir+DSite8[i][2])));
			}

			if(asum < 90)
			{
				continue;
			}
			
			// 求7个圈上8个方向上的角度变化和。中心点角度变化和应该为180，三角点为-180
			for(i = 0; i < 8; i++)
			{
				DAngleSum[i] = 0;
			}
			for(j = 1; j < 8; j++)
			{
				for(i = 0; i < 8; i++)
				{
					ad = AngleSub((sint32)(*(lpDir+DSite8[(i+1)%8][j])),
									(sint32)(*(lpDir+DSite8[i][j])));
					if(ad > 90 || ad < -90)
					{
						continue;
					}

					DAngleSum[j] += ad;
				}
			}
			// 判断是否为中心点。要求7个圈上的角度变化和都是180
			flag = true;
			for(i = 1; i < 8; i++)
			{
				if(DAngleSum[i] != 180)
				{
					flag = false;
					break;
				}
			}
			//是的话则记录下来其坐标
			if(flag && flag2)
			{
				if(CoreNum == 0)
				{
					core[CoreNum].x = x;
					core[CoreNum].y = y;
					core[CoreNum].Type = 1;
					CoreNum++;
				}
				else
				{
					// 检查在小距离内是否有中心点，有的话将它们坐标做平均做为中心点坐标
					fg = true;
					for(i = 0; i < CoreNum; i++)
					{
						dis = sqrt((double)((core[i].x/core[i].Type - x)*
							(core[i].x/core[i].Type - x) +
							(core[i].y/core[i].Type - y)*
							(core[i].y/core[i].Type - y)));
						if(dis < 8)
						{
							core[i].x += x;
							core[i].y += y;
							core[i].Type++;
							fg = false;
							break;
						}
					}
					if(fg)
					{
						core[CoreNum].x = x;
						core[CoreNum].y = y;
						core[CoreNum].Type = 1;
						CoreNum++;
						if(CoreNum > VF_MAXCORENUM)
						{
							flag2 = false;
						}

					}
				}
				continue;
			}
			// 判断是否为三角点。要求7个圈上的角度变化和都是-180

			flag = true;
			for(i = 1; i < 8; i++)
			{
				if(DAngleSum[i] != -180)
				{
					flag = false;
					break;
				}
			}
			if(flag && flag3)
			{
				//*lpTemp = 255;
				if(DeltaNum == 0)
				{
					delta[DeltaNum].x = x;
					delta[DeltaNum].y = y;
					delta[DeltaNum].Type = 1;
					DeltaNum++;
				}
				else
				{
					// 检查在小距离内是否有三角点，有的话将它们坐标做平均做为三角点坐标
					fg = true;
					for(i = 0; i < DeltaNum; i++)
					{
						dis = sqrt((double)((delta[i].x/delta[i].Type - x)*
							(delta[i].x/delta[i].Type - x) +
							(delta[i].y/delta[i].Type - y)*
							(delta[i].y/delta[i].Type - y)));
						if(dis < 8)
						{
							delta[i].x += x;
							delta[i].y += y;
							delta[i].Type++;
							fg = false;
							break;
						}
					}
					if(fg)
					{
						delta[DeltaNum].x = x;
						delta[DeltaNum].y = y;
						delta[DeltaNum].Type = 1;
						DeltaNum++;
						if(DeltaNum > VF_MAXDELTANUM)
						{
							flag3 = false;
						}

					}
				}
			}

		}
		temp += IMGW;
	}
	// 中心点太多则去掉多余的
	if(DeltaNum > VF_MAXDELTANUM)
	{
		DeltaNum = VF_MAXDELTANUM;
	}

	// 三角点太多则去掉多余的
	if(CoreNum > VF_MAXCORENUM)
	{
		CoreNum = VF_MAXCORENUM;
	}
	
	// 求中心点方向. 圆心与圆上所有点的连线与该点方向夹角最小，则中心点方向为该点方向
	for(i = 0; i < CoreNum; i++)
	{
		core[i].x /= core[i].Type;
		core[i].y /= core[i].Type;
		core[i].Type = VF_MINUTIA_CORE;
		lpCore = g_lpOrient + core[i].y*IMGW + core[i].x;
		mina = 360;
		for(j = 0; j < 100; j++)
		{
			lpDir = lpCore + SiteR15[j];
			temp = OriR15[j];
			if(temp >= 180)
				temp -=180;
			temp = GetJiajiao(temp, *lpDir);
			if(temp < mina)
			{
				mina = temp;
				minindex = j;
			}
		}
		// 夹角最小值要小于6度
		if(mina < 6)
		{
			lpDir = lpCore + SiteR15[minindex];
			if(OriR15[minindex] >= 180)
				core[i].Direction = (*lpDir + 180);
			else
				core[i].Direction = *lpDir;
		}
		else
		{
			core[i].Direction = 400;
		}
	}

	for(i = 0; i < DeltaNum; i++)
	{
		delta[i].x /= delta[i].Type;
		delta[i].y /= delta[i].Type;
		delta[i].Type = VF_MINUTIA_DELTA;
	}

	g_lpFeature->CoreNum = (uint8)CoreNum;
	g_lpFeature->DeltaNum = (uint8)DeltaNum;

	memcpy((void *)g_lpFeature->CoreArr, (void *)&core[0], CoreNum*sizeof(VF_MINUTIA));

	memcpy((void *)g_lpFeature->DeltaArr, (void *)&delta[0], DeltaNum*sizeof(VF_MINUTIA));

}


*/

#endif