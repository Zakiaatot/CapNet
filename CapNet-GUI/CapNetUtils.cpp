#include "CapNetGUIDlg.h"
#include <Windows.h>
#include <atlstr.h>
#include "CapNetUtils.h"


static INT AlertW(UINT option, const wchar_t* msg)
{
	return MessageBoxW(CCapNetGUIDlg::GetHwnd(), msg, L"CapNet-GUI", option | MB_APPLMODAL);
}

static INT AlertA(UINT option, const char* msg)
{
	return MessageBoxA(CCapNetGUIDlg::GetHwnd(), msg, "CapNet-GUI", option | MB_APPLMODAL);
}


void CapNetUtils::AlertErrorW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	AlertW(MB_OK | MB_ICONSTOP, buf);
}

void CapNetUtils::AlertErrorA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	AlertA(MB_OK | MB_ICONSTOP, buf);
}

void CapNetUtils::AlertWarningW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	AlertW(MB_OK | MB_ICONWARNING, buf);
}

void CapNetUtils::AlertWarningA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	AlertA(MB_OK | MB_ICONWARNING, buf);
}

void CapNetUtils::AlertInfoW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	AlertW(MB_OK | MB_ICONINFORMATION, buf);
}

void CapNetUtils::AlertInfoA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	AlertA(MB_OK | MB_ICONINFORMATION, buf);
}

int CapNetUtils::AlertQueW(const wchar_t* msg, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	wvsprintfW(buf, msg, args);
	va_end(args);
	return AlertW(MB_YESNO | MB_ICONQUESTION, buf);
}

int CapNetUtils::AlertQueA(const char* msg, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, msg, args);
	va_end(args);
	return AlertA(MB_YESNO | MB_ICONQUESTION, buf);
}