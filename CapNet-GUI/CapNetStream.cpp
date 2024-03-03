#include <cstdarg>
#include <Windows.h>
#include "CapNetStream.h"

std::string CapNetOutStreamA::Format(const char* str, ...)
{
	CHAR buf[256] = { 0 };
	va_list args;
	va_start(args, str);
	vsprintf_s(buf, str, args);
	va_end(args);
	return buf;
}

std::string CapNetOutStreamA::VFormat(const char* str, va_list args)
{
	CHAR buf[256] = { 0 };
	wvsprintfA(buf, str, args);
	return buf;
}

std::wstring CapNetOutStreamW::Format(const wchar_t* str, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, str);
	wvsprintfW(buf, str, args);
	va_end(args);
	return buf;
}

std::wstring CapNetOutStreamW::VFormat(const wchar_t* str, va_list args)
{
	WCHAR buf[256] = { 0 };
	wvsprintfW(buf, str, args);
	return buf;
}
