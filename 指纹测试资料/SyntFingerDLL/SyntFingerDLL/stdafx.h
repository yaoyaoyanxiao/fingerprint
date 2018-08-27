// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D4AB2DAD_02A2_4045_9A6A_0EC0682A8F29__INCLUDED_)
#define AFX_STDAFX_H__D4AB2DAD_02A2_4045_9A6A_0EC0682A8F29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
//点类型，坐标值为浮点
typedef  struct DblPoint
{
	double x;
	double y;
} DBLPOINT;

//点类型，坐标值为长整数
/*typedef struct tagPOINT
{
long  x;
long  y;
} POINT, *PPOINT, NEAR *NPPOINT, FAR *LPPOINT;
*/

typedef POINT *PPOINT;

#define	MAX_SINGULARYNUM	30
// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D4AB2DAD_02A2_4045_9A6A_0EC0682A8F29__INCLUDED_)
