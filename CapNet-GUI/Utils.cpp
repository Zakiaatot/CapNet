#include <Windows.h>
#include <atlstr.h>
#include "Utils.h"



static INT AlertW(UINT option, const wchar_t* msg)
{
	return MessageBoxW(NULL, msg, L"CapNet-GUI", option);
}

static INT AlertA(UINT option, const char* msg)
{
	return MessageBoxA(NULL, msg, "CapNet-GUI", option);
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

int Utils::AlertQueW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	return AlertW(MB_YESNO | MB_ICONQUESTION, buf);
}

int Utils::AlertQueA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	return AlertA(MB_YESNO | MB_ICONQUESTION, buf);
}