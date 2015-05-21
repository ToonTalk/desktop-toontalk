// Copyright (c) 1992,1999.  Ken Kahn, Animated Programs, All rights reserved
  
// don't know a better way to make tt_error_file() easily available
// to all files
#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_CONSTANT_H)   
#include "constant.h"
#endif 


// here's what Microsoft does:
/*
inline void * __cdecl operator new(size_t s)
        { return ::operator new(s, _NORMAL_BLOCK, __FILE__, __LINE__); }

inline void* __cdecl operator new[](size_t s)
        { return ::operator new[](s, _NORMAL_BLOCK, __FILE__, __LINE__); }
		  */


#if _DEBUG

void* operator new(size_t nSize, const char * lpszFileName, int nLine)
{
    return ::operator new(nSize, 1, lpszFileName, nLine);
}

#endif

   

//#if TT_UNICODE
output_file_stream tt_err_file;
//#else
//output_file_stream tt_error_file();
//#endif

#if TT_SPECIAL_DEBUGGING
output_file_stream tt_special_debugging_file;
#endif
