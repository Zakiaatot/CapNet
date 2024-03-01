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
	void AlertQueW(const wchar_t* msg, ...);
	void AlertQueA(const char* msg, ...);

	// Check Ip Address
	bool CheckIpAddressW(const wchar_t* ip);
	bool CheckIpAddressA(const char* ip);
}

#endif // !_UTILS_H_


