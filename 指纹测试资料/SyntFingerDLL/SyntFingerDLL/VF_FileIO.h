//#define _CRT_SECURE_NO_WARNINGS

#ifndef	__VFFILEIO_H__
#define __VFFILEIO_H__

#include "VF_Type.h"
#include "VF_Global.h"

//
//	loadBitmap: 从BMP文件中读取图像数据
sint32	loadBitmap(sint8 *lpszFileName, uint8 *lpBitmap, sint32 *lpWidth, sint32 *lpHeight)
{
/////////////////////////////////////////////////////////////////////
//  lpszFileName: [in] BMP文件名
//	lpBitmap: [out] 读出的图像数据
//  lpWidth: [out] 图像宽度
//	lpHeight: [out] 图像高度
/////////////////////////////////////////////////////////////////////
   	FILE               *fp = NULL;
	uint8			   palatte[1024];
	BITMAPFILEHEADER   bf;
	BITMAPINFOHEADER   bi;
	sint32			   i;
	sint32             lineBytes = 0;
	sint32             Width;
	sint32             Height;
	sint32			   Size = 0;
	
	// 打开文件
	fp = fopen(lpszFileName, "rb");
	if(fp == NULL)
	{
		return 1;
	}
	// 读文件头
	fread((void *)&bf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread((void *)&bi, sizeof(BITMAPINFOHEADER), 1, fp);
	// 检查是否为BMP文件
	if(bf.bfType != 19778)
	{
		fclose(fp);
		return 2;
	}
	// 检查是否为256色灰度图像
	if(bi.biBitCount != 8 && bi.biClrUsed != 256)
	{
		fclose(fp);
		return 3;
	}
	// 得到图像大小
	Height = bi.biHeight;
	Width = bi.biWidth;
	
	// 缓冲区指针不为空则读取图像数据到缓冲区
	if(lpBitmap != NULL)
	{
		lineBytes = (sint32)WIDTHBYTES(Width);
		Size = lineBytes*Height;
		
		fread(palatte, 1, 1024, fp);
		// 按行读取，每行只读取Width个字节
		for(i = 0; i < Height; i++)
		{
			fseek(fp, 1078+lineBytes*i, SEEK_SET);
			fread((void *)(lpBitmap+i*Width), 1, Width, fp);
		}
	}
	fclose(fp);	

	*lpWidth = Width;
	*lpHeight = Height;

	return 0;
}

//
//	saveBitmap: 将图像数据保存成BMP文件
sint32	saveBitmap(uint8 *lpBitmap, sint32 Width, sint32 Height, sint8 *lpszFileName)
{
/////////////////////////////////////////////////////////////////////
//	lpBitmap: [in] 要保存的图像数据
//  Width: [in] 图像宽度
//	Height: [in] 图像高度
//  lpszFileName: [in] BMP文件名
/////////////////////////////////////////////////////////////////////
   	FILE               *fp = NULL;
	uint8				palatte[1024];
	BITMAPFILEHEADER   bf;
	BITMAPINFOHEADER   bi;
	sint32             lineBytes = 0;
	sint32             i;
	
	lineBytes = (sint32)WIDTHBYTES(Width);
	
	//构造bmp文件头信息
	bf.bfType=19778;
	bf.bfSize = lineBytes*Height+1078;
	bf.bfReserved1=0;
	bf.bfReserved2=0;
	bf.bfOffBits=1078;
	bi.biSize=40;
	bi.biWidth = Width;
	bi.biHeight = Height;
	bi.biPlanes=1;
	bi.biBitCount=8;
	bi.biCompression=0;
	bi.biSizeImage= lineBytes*Height;
	bi.biXPelsPerMeter=0;
	bi.biYPelsPerMeter=0;
	bi.biClrUsed=256;

	bi.biClrImportant=0;
	//构造调色板数据
	for(i=0;i<256;i++){
		palatte[i*4]=(unsigned char)i;
		palatte[i*4+1]=(unsigned char)i;
		palatte[i*4+2]=(unsigned char)i;
		palatte[i*4+3]=0;
	}	
	// 创建文件
	fp = fopen(lpszFileName, "wb");
	if(fp == NULL)
	{
		return 1;
	}
	// 写文件头
	fwrite((char *)&bf,sizeof(BITMAPFILEHEADER), 1, fp); 
	// 写BMP图像信息头
	fwrite((char *)&bi, sizeof(BITMAPINFOHEADER), 1, fp);
	// 写调色板
	fwrite((char *)palatte, sizeof(palatte), 1, fp);
	// 按行写，每行写WIDTHBYTES(Width)个字节
	for(i = 0; i < Height; i++)
		fwrite((char *)(lpBitmap+i*Width), lineBytes, 1, fp);

	fclose(fp);	

	return 0;
}



#endif