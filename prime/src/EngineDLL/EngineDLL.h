// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the ENGINEDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// ENGINEDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ENGINEDLL_EXPORTS
#define ENGINEDLL_API __declspec(dllexport)
#else
#define ENGINEDLL_API __declspec(dllimport)
#endif

// This class is exported from the dll
class ENGINEDLL_API CEngineDLL {
public:
	CEngineDLL(void);
	// TODO: add your methods here.
};

extern ENGINEDLL_API int nEngineDLL;

ENGINEDLL_API int fnEngineDLL(void);
