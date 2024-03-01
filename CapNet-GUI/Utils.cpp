#include <stdio.h>
#include <string>
#include <regex>
#include <Windows.h>
#include <atlstr.h>
#include "Utils.h"



static void AlertW(UINT option, const wchar_t* msg)
{
	MessageBoxW(NULL, msg, L"CapNet", option);
}

static void AlertA(UINT option, const char* msg)
{
	MessageBoxA(NULL, msg, "CapNet", option);
}


void Utils::AlertErrorW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	AlertW(MB_OK | MB_ICONSTOP, buf);
}

void Utils::AlertErrorA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	AlertA(MB_OK | MB_ICONSTOP, buf);
}

void Utils::AlertWarningW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	AlertW(MB_OK | MB_ICONWARNING, buf);
}

void Utils::AlertWarningA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	AlertA(MB_OK | MB_ICONWARNING, buf);
}

void Utils::AlertInfoW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	AlertW(MB_OK | MB_ICONINFORMATION, buf);
}

void Utils::AlertInfoA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	AlertA(MB_OK | MB_ICONINFORMATION, buf);
}

void Utils::AlertQueW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	AlertW(MB_OK | MB_ICONQUESTION, buf);
}

void Utils::AlertQueA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	AlertA(MB_OK | MB_ICONQUESTION, buf);
}

bool Utils::CheckIpAddressW(const wchar_t* ip)
{
	std::wstring ipAddress(ip);
	// ������ʽģʽ������ƥ��IPv4��ַ
	std::wstring ipv4Pattern = LR"(\b(?:\d{1,3}\.){3}\d{1,3}\b)";
	// ������ʽģʽ������ƥ��IPv6��ַ
	std::wstring ipv6Pattern = LR"(\b(?:[A-Fa-f0-9]{1,4}:){7}[A-Fa-f0-9]{1,4}\b)";
	// ���IPv4��ַ
	std::wregex ipv4Regex(ipv4Pattern);
	if (std::regex_match(ipAddress, ipv4Regex))
		return true;
	// ���IPv6��ַ
	std::wregex ipv6Regex(ipv6Pattern);
	if (std::regex_match(ipAddress, ipv6Regex))
		return true;

	return false;
}

bool Utils::CheckIpAddressA(const char* ip)
{
	CA2W ca2w(ip);
	std::wstring ws(ca2w);
	return Utils::CheckIpAddressW(ws.c_str());
}
