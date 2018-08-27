#ifndef	__VFTHIN_H__
#define __VFTHIN_H__

#include "VF_Type.h"
#include "VF_Global.h"
#include "VF_Function.h"

//
//	imageThin: 图像细化处理
//
sint32 imageThin(uint8 *lpBits, sint32 Width, sint32 Height)
{
/////////////////////////////////////////////////////////////////
//	lpBits: [in, out] 要细化的图像数据缓冲区 
//	Width: [in] 要细化的图像宽度
//	Height: [in] 要细化的图像高度
/////////////////////////////////////////////////////////////////
	uint8  erasetable[256]={
			0,0,1,1,0,0,1,1,             1,1,0,1,1,1,0,1,
			1,1,0,0,1,1,1,1,             0,0,0,0,0,0,0,1,
			0,0,1,1,0,0,1,1,             1,1,0,1,1,1,0,1,
			1,1,0,0,1,1,1,1,             0,0,0,0,0,0,0,1,
			1,1,0,0,1,1,0,0,             0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,             0,0,0,0,0,0,0,0,
			1,1,0,0,1,1,0,0,             1,1,0,1,1,1,0,1,
			0,0,0,0,0,0,0,0,             0,0,0,0,0,0,0,0,
			0,0,1,1,0,0,1,1,             1,1,0,1,1,1,0,1,
			1,1,0,0,1,1,1,1,             0,0,0,0,0,0,0,1,
			0,0,1,1,0,0,1,1,             1,1,0,1,1,1,0,1,
			1,1,0,0,1,1,1,1,             0,0,0,0,0,0,0,0,
			1,1,0,0,1,1,0,0,             0,0,0,0,0,0,0,0,
			1,1,0,0,1,1,1,1,             0,0,0,0,0,0,0,0,
			1,1,0,0,1,1,0,0,             1,1,0,1,1,1,0,0,
			1,1,0,0,1,1,1,0,             1,1,0,0,1,0,0,0
	};
	sint32		x,y;
	sint32      num;
	BOOL        Finished;
	uint8       nw,n,ne,w,e,sw,s,se;
	uint8       *lpPtr = NULL;
	uint8       *lpTempPtr = NULL;	

	memcpy((void *)g_lpTemp, (void *)lpBits, Width*Height);
	
	//结束标志置成假
	Finished=FALSE;
	while(!Finished){ //还没有结束
              //结束标志置成假
            Finished=TRUE;
       //先进行水平方向的细化
              for (y=1;y<Height-1;y++)
			  { //注意为防止越界，y的范围从1到高度-2
                     //lpPtr指向原图数据，lpTempPtr指向新图数据

                     lpPtr=(uint8 *)lpBits+y*Width;
                     lpTempPtr=(uint8 *)g_lpTemp+y*Width;

                     x=1; //注意为防止越界，x的范围从1到宽度-2

                     while(x<Width-1)
					 {
                            if(*(lpPtr+x)==0)
							{ //是黑点才做处理
                                   w = *(lpPtr+x-1);  //左邻点
                                   e = *(lpPtr+x+1);  //右邻点

                                   if( (w==255)|| (e==255)){ 

									//如果左右两个邻居中至少有一个是白点才处理

                                          nw=*(lpPtr+x+Width-1); //左上邻点

                                          n=*(lpPtr+x+Width); //上邻点

                                          ne=*(lpPtr+x+Width+1); //右上邻点

                                          sw=*(lpPtr+x-Width-1); //左下邻点

                                          s=*(lpPtr+x-Width); //下邻点

                                          se=*(lpPtr+x-Width+1); //右下邻点

                                          //计算索引

                            num=nw/255+n/255*2+ne/255*4+w/255*8+e/255*16+
								sw/255*32+s/255*64+se/255*128;

                                          if(erasetable[num]==1){ //经查表，可以删除

												//在原图缓冲区中将该黑点删除

                                                 *(lpPtr+x)=255; 

													//结果图中该黑点也删除

                                                 *(lpTempPtr+x)=255; 

                                                 Finished=FALSE; //有改动，结束标志置成假

                                                 x++; //水平方向跳过一个象素

                                          }

                                   }

                            }

                            x++; //扫描下一个象素

                     }

              }

       //再进行垂直方向的细化

              for (x=1;x<Width-1;x++){ //注意为防止越界，x的范围从1到宽度-2

                     y=1; //注意为防止越界，y的范围从1到高度-2

                     while(y<Height-1){

                            lpPtr=lpBits+y*Width;

                            lpTempPtr=g_lpTemp+y*Width;

                            if(*(lpPtr+x)==0){ //是黑点才做处理

                                   n=*(lpPtr+x+Width);

                                   s=*(lpPtr+x-Width);

                                   if( (n==255)|| (s==255)){

//如果上下两个邻居中至少有一个是白点才处理

                                          nw=*(lpPtr+x+Width-1);

                                          ne=*(lpPtr+x+Width+1);

                                          w=*(lpPtr+x-1);

                                          e=*(lpPtr+x+1);

                                          sw=*(lpPtr+x-Width-1);

                                          se=*(lpPtr+x-Width+1);

                                          //计算索引

										num=nw/255+n/255*2+ne/255*4+w/255*8+e/255*16+

											sw/255*32+s/255*64+se/255*128;

                                          if(erasetable[num]==1){ //经查表，可以删除

//在原图缓冲区中将该黑点删除

                                                 *(lpPtr+x)=255; 

//结果图中该黑点也删除

                                                 *(lpTempPtr+x)=255; 

                                                 Finished=FALSE; //有改动，结束标志置成假

                                                 y++;//垂直方向跳过一个象素

                                          }

                                   }

                            }

                            y++; //扫描下一个象素

                     }

              } 

	}
	
	memcpy((void *)lpBits, (void *)g_lpTemp, Width*Height);

	return 0;
}

//
//	thin : 对当前指纹图像进行细化处理
//
sint32 thin()
{
	imageThin(g_lpOrgFinger, IMGW, IMGH);

	return 0;
}

//
//	thinClear: 清除细化图像中短棒和毛刺
//
sint32  thinClear(sint32  len)
{
/////////////////////////////////////////////////////////////////
//	len: [in] 短棒和毛刺的最大长度
/////////////////////////////////////////////////////////////////
	sint32	x, y;
	sint32  i, n, num;
	sint32  temp;
	uint8   *Line[25];
	uint8   *lpNow = NULL;
	uint8   *lpLast = NULL;
	uint8   *lpNext = NULL;
	uint8   *tempPtr[8];
	sint32	SiteD8[8] = {IMGW-1, IMGW, IMGW+1, 1, -IMGW+1, -IMGW, -IMGW-1, -1};

	temp = 0;
	for(y = 0; y < IMGH; y++)
	{
		for(x = 0; x < IMGW; x++)
		{
			lpNow = g_lpOrgFinger + temp + x;
			if(*lpNow != 0)
			{
				continue;
			}
			Line[0] = lpNow;
			// 统计当前点的周围黑点个数
			n = 0;
			for(i = 0; i < 8; i++)
			{
				lpNext = lpNow + SiteD8[i];
				if(*lpNext == 0)
				{
					tempPtr[n] = lpNext;
					n++;
				}
			}
			// 黑点个数为零，表示当前点是孤点，置为白色
			if(n == 0)
			{
				*lpNow = 255;
				continue;
			}
			// 黑点个数为1，表示为端点
			else if(n == 1)
			{
				num = 0;
				lpLast = lpNow;
				lpNow = tempPtr[0];
				// 沿纹线跟踪len个点
				for(i = 0; i < len; i++)
				{
					// 如果遇到叉点则跳出循环
					if(IsFork(lpNow))
					{
						break;
					}
					num++;
					Line[num] = lpNow;
					if(GetNext(lpNow, lpLast, &lpNext) == 0)
					{
						lpLast = lpNow;
						lpNow = lpNext;
					}
					else // 如果遇到异常跳出循环
					{
						break;
					}

				}
				
				// 纹线较短，表示为短棒或者是毛刺
				if(num < len)
				{
					for(i = 0; i <= num; i++)
					{
						*Line[i] = 255;
					}
				}

			}
		}
		temp += IMGW;
	}

	return 0;
}

#endif