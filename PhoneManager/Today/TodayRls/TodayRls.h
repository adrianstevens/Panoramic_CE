// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TODAYRLS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TODAYRLS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TODAYRLS_EXPORTS
#define TODAYRLS_API __declspec(dllexport)
#else
#define TODAYRLS_API __declspec(dllimport)
#endif

TODAYRLS_API void LaunchOptions(HWND hWndParent);
