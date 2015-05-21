
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the JPNT32_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// JPNT32_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef JPNT32_EXPORTS
#define JPNT32_API __declspec(dllexport)
#else
#define JPNT32_API __declspec(dllimport)
#endif

// This class is exported from the jpnt32.dll
class JPNT32_API CJpnt32 {
public:
	CJpnt32(void);
	// TODO: add your methods here.
};

extern JPNT32_API int nJpnt32;

JPNT32_API int fnJpnt32(void);

