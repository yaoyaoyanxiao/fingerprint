#ifndef	__VFTYPE_H__
#define __VFTYPE_H__

/* Basic Types */
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef short sint16;
typedef unsigned long uint32;
typedef int sint32;
//typedef long sint32;
typedef char sint8;

#if defined(WIN32)
typedef unsigned __int64 uint64; /* MSVC++ 5/6 declaration */
#else
#ifdef __arch64__
typedef unsigned long uint64;
#else
typedef unsigned long long uint64; /* gcc 2.7.2 declaration */
#endif
#endif

typedef struct tagMatchResult { 
  sint32    Similarity; 
  sint32    Rotation; 
  sint32    TransX;
  sint32    TransY;
  sint32	MMCount;
} MATCHRESULT, *PMATCHRESULT;                                                                                  


/*
	VF_RETURN
	This data type is returned by most functions.
*/
typedef uint32 VF_RETURN;

typedef uint8  VF_FLAG;


#define VF_OK (0)

/*
	VF_MINUTIAE_TYPE
*/

#define VF_MINUTIA_END        1	// 端点
#define VF_MINUTIA_FORK       2	// 叉点
#define VF_MINUTIA_CORE		  3 //中心点
#define VF_MINUTIA_DELTA	  4 //三角点

// 精确比对模式，主要用在少量比对次数场合
#define	VF_MATCHMODE_VERIFY			1
// 快速比对模式，主要用在大量比对次数场合
#define	VF_MATCHMODE_IDENTIFY		2



#endif
