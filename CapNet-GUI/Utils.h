#ifndef _UTILS_H_
#define _UTILS_H_

namespace Utils
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
}

#endif // !_UTILS_H_


