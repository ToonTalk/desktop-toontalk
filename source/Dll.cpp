// Copyright (c) 1992,1999.  Ken Kahn, Animated Programs, All rights reserved

#include <windows.h>

//extern "C" {
//int FAR PASCAL _export WEP (int nParam) ;
//}

extern "C" int __declspec(dllexport) WEP (int nParam); // copied here on 151203 from ttfile.cpp


int FAR PASCAL LibMain (HANDLE , WORD , WORD //wHeapSize
								, LPSTR ) {
//	  if (wHeapSize > 0)
//			 UnlockData (0) ;

	  return 1 ;
}

//int FAR PASCAL _export WEP (int) {
//	  return 1 ;
//}

int __declspec(dllexport) WEP (int) { // copied here on 151203 from ttfile.cpp
	  return 1 ;
}
