#ifndef _CAPNET_STREAM_H_
#define _CAPNET_STREAM_H_

#include <sstream>

class CapNetOutStreamA :public std::ostringstream
{
public:
	operator std::string() { return std::ostringstream::str(); }
	static std::string Format(const char* str, ...);
};

class CapNetOutStreamW :public std::wostringstream
{
public:
	operator std::wstring() { return std::wostringstream::str(); }
	static std::wstring Format(const wchar_t* str, ...);
};

#endif // !_CAPNET_STREAM_H_


