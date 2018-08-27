#ifndef	__VFGLOBAL_H__
#define __VFGLOBAL_H__

#include "VF_Type.h"

#include <stdlib.h>
#include <stdio.h>
#include "math.h"

#define		MAXIMGW				640		// 处理图像最大宽度
#define		MAXIMGH				480		// 处理图像最大高度
#define		MAXIMGSIZE			(MAXIMGW*MAXIMGH) // 处理图像最大大小

sint32  	IMGW = 256;		// 当前处理图像的宽度
sint32      IMGH = 360;		// 当前处理图像的高度
sint32      IMGSIZE = (IMGW*IMGH);	// 当前处理图像的大小


uint8		g_OrgFinger[MAXIMGSIZE];	// 处理图像数据缓冲区
uint8		g_Orient[MAXIMGSIZE];		// 方向场数据缓冲区
uint8		g_Divide[MAXIMGSIZE];		// 分割图数据缓冲区
uint8		g_Temp[MAXIMGSIZE];			// 临时数据缓冲区

uint8		*g_lpOrgFinger = &g_OrgFinger[0];	// 处理图像数据缓冲区首地址
uint8		*g_lpOrient = &g_Orient[0];			// 方向场数据缓冲区首地址
uint8		*g_lpDivide = &g_Divide[0];			// 分割图数据缓冲区首地址
uint8		*g_lpTemp = &g_Temp[0];				// 临时数据缓冲区首地址

#define     MAXMINUTIANUM		72
// 最大特征点数
#define     MAXRAWMINUTIANUM        100			// 包含虚假特征点的初始特征点最大数目

sint32      g_MinutiaNum = 0;					// 特征点数之和

#define  PI          3.141593
#define  EPI         57.29578

// 以某点为中心的12个量化方向上7个点位置的横纵偏移
sint32  g_DSite[12][7][2] = {
	-3, 0,  -2, 0,  -1, 0,   0, 0,   1, 0,   2, 0,   3, 0,
	-3,-1,  -2,-1,  -1, 0,   0, 0,   1, 0,   2, 1,   3, 1,
	-3,-2,  -2,-1,  -1,-1,   0, 0,   1, 1,   2, 1,   3, 2,
	-3,-3,  -2,-2,  -1,-1,   0, 0,   1, 1,   2, 2,   3, 3,
	-2,-3,  -1,-2,  -1,-1,   0, 0,   1, 1,   1, 2,   2, 3,
	-1,-3,  -1,-2,   0,-1,   0, 0,   0, 1,   1, 2,   1, 3,
	 0,-3,   0,-2,   0,-1,   0, 0,   0, 1,   0, 2,   0, 3,
	-1, 3,  -1, 2,   0, 1,   0, 0,   0,-1,   1,-2,   1,-3,
	-2, 3,  -1, 2,  -1, 1,   0, 0,   1,-1,   1,-2,   2,-3,
	-3, 3,  -2, 2,  -1, 1,   0, 0,   1,-1,   2,-2,   3,-3,
	-3, 2,  -2, 1,  -1, 1,   0, 0,   1,-1,   2,-1,   3,-2,
	-3, 1,  -2, 1,  -1, 0,   0, 0,   1, 0,   2,-1,   3,-1
};

// 特征点结构
typedef struct tagMinutiae { 
  sint32    x;				// 横坐标
  sint32    y;				// 纵坐标
  sint32    Direction;		// 方向
  sint32	Triangle[3];	// 特征点为中心外接圆半径为定值的正三角形三个顶点的方向
  sint32    Type;			// 类型
} MINUTIA, *MINUTIAPTR; 

// 指纹特征(模板)结构
typedef struct tagFeature{
	sint32		MinutiaNum;					// 特征点数
	MINUTIA		MinutiaArr[MAXMINUTIANUM];	// 特征点数组
} FEATURE, *FEATUREPTR;


FEATURE    g_Feature;		// 当前指纹的指纹特征

#define SAFE_FREE(x) { if (x != NULL) free((void *)x); x = NULL; }

#define WIDTHBYTES(i)    ((i+3)/4*4)


#endif