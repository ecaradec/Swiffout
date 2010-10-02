// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the REGPROTOCOL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// REGPROTOCOL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef REGPROTOCOL_EXPORTS
#define REGPROTOCOL_API __declspec(dllexport)
#else
#define REGPROTOCOL_API __declspec(dllimport)
#endif

// This class is exported from the regProtocol.dll
class REGPROTOCOL_API CregProtocol {
public:
	CregProtocol(void);
	// TODO: add your methods here.
};

extern REGPROTOCOL_API int nregProtocol;

REGPROTOCOL_API int fnregProtocol(void);
