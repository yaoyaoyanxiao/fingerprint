int __stdcall Add(int numa,int numb);

int __stdcall Analyze(BYTE *lpImage, int Width, int Height, BYTE *lpFeature, int *lpSize);

int __stdcall AnalyzeFromFile(LPCSTR lpszFileName, BYTE *lpFeature, int *lpSize);

int __stdcall PatternMatch(BYTE *lpFeature1, BYTE *lpFeature2, int *lpScore);

int __stdcall SaveFeature(LPCSTR lpszFileName, BYTE *lpFeature);
int __stdcall LoadFeature(LPCSTR lpszFileName, BYTE *lpFeature, int *lpSize);

int __stdcall LoadFingerImage(LPCSTR lpszFileName, BYTE *lpFinger, int *lpWidth, int *lpHeight);

int __stdcall SaveFingerImage(LPCSTR lpszFileName, BYTE *lpFinger, int Width, int Height);

int __stdcall GetImageQuality(BYTE *lpFinger, int Width, int Height, int *pScore);

