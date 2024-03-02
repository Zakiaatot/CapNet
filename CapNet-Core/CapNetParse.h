#ifndef _CAPNET_PARSE_H_
#define _CAPNET_PARSE_H_

#include <Windows.h>
#include <string>

namespace CapNetParse
{
	BOOL IsIpv6(const UCHAR* data);
	BOOL IsIpv4(const UCHAR* data);
	BOOL IsUdp(const UCHAR* data);
	BOOL IsTcp(const UCHAR* data);
	BOOL IsIcmp(const UCHAR* data);
	BOOL IsArp(const UCHAR* data);
	BOOL IsHttp(const UCHAR* data);
	BOOL IsHttps(const UCHAR* data);
	std::pair<std::wstring, std::wstring> GetProtocolAndInfo(const UCHAR* data);
	std::pair<std::wstring, std::wstring> GetSourceDestIp(const UCHAR* data);
	std::wstring GetTcpInfo(const UCHAR* data);
	std::wstring GetUdpInfo(const UCHAR* data);
	std::wstring GetIcmpInfo(const UCHAR* data);
	std::wstring GetIpv4Info(const UCHAR* data);
	std::wstring GetEtherInfo(const UCHAR* data);
	BOOL CompareProtocolW(const UCHAR* data, const WCHAR* protocol);
	BOOL CompareIpW(const UCHAR* data, const WCHAR* ip);
}

#endif // ! _CAPNET_PARSE_H_


