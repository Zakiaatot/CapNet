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

std::wstring CapNetOutStreamW::Format(const wchar_t* str, ...)
{
	WCHAR buf[256] = { 0 };
	va_list args;
	va_start(args, str);
	wvsprintfW(buf, str, args);
	va_end(args);
	return buf;
}
