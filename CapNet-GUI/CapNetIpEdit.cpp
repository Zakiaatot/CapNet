#include <regex>
#include "CapNetIpEdit.h"

BEGIN_MESSAGE_MAP(CapNetIpEdit, CComboBox)

END_MESSAGE_MAP()

VOID CapNetIpEdit::Init()
{
	return VOID();
}

BOOL CapNetIpEdit::CheckIpAddressW(const WCHAR* ip)
{
	std::wstring ipAddress(ip);
	// ������ʽģʽ������ƥ��IPv4��ַ
	std::wstring ipv4Pattern = LR"(\b(?:\d{1,3}\.){3}\d{1,3}\b)";
	// ������ʽģʽ������ƥ��IPv6��ַ
	std::wstring ipv6Pattern = LR"(\b(?:[A-Fa-f0-9]{1,4}:){7}[A-Fa-f0-9]{1,4}\b)";
	// ���IPv4��ַ
	std::wregex ipv4Regex(ipv4Pattern);
	if (std::regex_match(ipAddress, ipv4Regex))
		return TRUE;
	// ���IPv6��ַ
	std::wregex ipv6Regex(ipv6Pattern);
	if (std::regex_match(ipAddress, ipv6Regex))
		return TRUE;

	return FALSE;
}

BOOL CapNetIpEdit::CheckIpAddressA(const CHAR* ip)
{
	CA2W ca2w(ip);
	std::wstring ws(ca2w);
	return CheckIpAddressW(ws.c_str());
}
