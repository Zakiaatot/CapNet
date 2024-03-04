#ifndef _CAPNET_STREAM_H_
#define _CAPNET_STREAM_H_

#include <sstream>
#include <iomanip> 

class CapNetOutStreamA :public std::ostringstream
{
public:
	operator std::string() { return std::ostringstream::str(); }
	static std::string Format(const char* str, ...);
	static std::string VFormat(const char* str, va_list args);
};

class CapNetOutStreamW :public std::wostringstream
{
public:
	operator std::wstring() { return std::wostringstream::str(); }
	static std::wstring Format(const wchar_t* str, ...);
	static std::wstring VFormat(const wchar_t* str, va_list args);
};

#endif // !_CAPNET_STREAM_H_


