#ifndef _CAPNET_UTILS_H_
#define _CAPNET_UTILS_H_

#include "Windows.h"

namespace CapNetUtils
{
	// Alert
	void AlertErrorW(const wchar_t* msg, ...);
	void AlertErrorA(const char* msg, ...);
	void AlertWarningW(const wchar_t* msg, ...);
	void AlertWarningA(const char* msg, ...);
	void AlertInfoW(const wchar_t* msg, ...);
	void AlertInfoA(const char* msg, ...);
	int AlertQueW(const wchar_t* msg, ...);
	int AlertQueA(const char* msg, ...);

	// Release
	BOOL ReleaseResource
	(
		IN UINT uResourceId,
		IN PCWCHAR szResourceType,
		IN PCWCHAR szFileName,
		IN PCWCHAR selfDllName,
		IN OUT PWCHAR errMsg
	);
}

#endif // !_CAPNET_UTILS_H_


