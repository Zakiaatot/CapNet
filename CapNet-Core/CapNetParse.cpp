#include <atlstr.h>
#include <vector>
#include <ws2tcpip.h>
#include "CapNetParse.h"
#include "CapNetProtocol.h"
#include "CapNetStream.h"

#pragma comment(lib,"Ws2_32.lib")

BOOL CapNetParse::IsIpv6(const UCHAR* data)
{
	EtherHeader* ethHdr = (EtherHeader*)data;
	USHORT ethType = ntohs(ethHdr->etherType);
	if (ethType == 0x86DD)
		return TRUE;
	else
		return FALSE;
}

BOOL CapNetParse::IsIpv4(const UCHAR* data)
{
	EtherHeader* ethHdr = (EtherHeader*)data;
	USHORT ethType = ntohs(ethHdr->etherType);
	if (ethType == 0x0800)
		return TRUE;
	else
		return FALSE;
}

BOOL CapNetParse::IsUdp(const UCHAR* data)
{
	if (IsIpv4(data))
	{
		IpHeader* iph = (IpHeader*)(data + 14);
		return iph->protocol == 0x11;
	}
	if (IsIpv6(data))
	{
		Ipv6Header* iph = (Ipv6Header*)(data + 14);
		return iph->nextHeader == 0x11;
	}
	return FALSE;
}

BOOL CapNetParse::IsTcp(const UCHAR* data)
{
	if (IsIpv4(data))
	{
		IpHeader* iph = (IpHeader*)(data + 14);
		return iph->protocol == 0x06;
	}
	if (IsIpv6(data))
	{
		Ipv6Header* iph = (Ipv6Header*)(data + 14);
		return iph->nextHeader == 0x06;
	}
	return FALSE;
}

BOOL CapNetParse::IsIcmp(const UCHAR* data)
{
	if (IsIpv4(data))
	{
		IpHeader* iph = (IpHeader*)(data + 14);
		return iph->protocol == 0x01;
	}
	if (IsIpv6(data))
	{
		Ipv6Header* iph = (Ipv6Header*)(data + 14);
		return iph->nextHeader == 0x01;
	}
	return FALSE;
}

BOOL CapNetParse::IsArp(const UCHAR* data)
{
	EtherHeader* ethHdr = (EtherHeader*)data;
	USHORT ethType = ntohs(ethHdr->etherType);
	if (ethType == 0x0806)
		return TRUE;
	else
		return FALSE;
}

BOOL CapNetParse::IsHttp(const UCHAR* data)
{
	if (!IsTcp(data))
		return FALSE;
	const TcpHeader* tcpHeader = (const TcpHeader*)(data + 14 + 20);
	if (ntohs(tcpHeader->destPort) == 80 || ntohs(tcpHeader->sourPort) == 80)
	{
		const char* http = (const char*)(data + 14 + 20 + 20);
		const std::vector<std::string> httpMethods =
		{
			"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "TRACE", "CONNECT", "PATCH"
		};

		for (const std::string& method : httpMethods) {
			if (strstr(http, method.c_str()) != nullptr) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CapNetParse::IsHttps(const UCHAR* data)
{
	if (!IsTcp(data))
		return FALSE;
	const TcpHeader* tcpHeader = (TcpHeader*)(data + 14 + 20);
	if (ntohs(tcpHeader->destPort) == 443 || ntohs(tcpHeader->sourPort) == 443)
	{
		return TRUE;
	}
	return FALSE;
}

std::pair<std::wstring, std::wstring> CapNetParse::GetProtocolAndInfo(const UCHAR* data)
{
	std::wstring protocol, info;
	if (IsIpv4(data))
	{
		// ipv4
		if (IsTcp(data))
		{
			// tcp
			if (IsHttp(data))
			{
				// http
				protocol = L"Http";
				info = GetHttpInfo(data);
			}
			else if (IsHttps(data))
			{
				// https
				protocol = L"Https";
				//info = L"To do";
				info = GetTcpInfo(data);
			}
			else
			{
				protocol = L"Tcp";
				info = GetTcpInfo(data);
			}
		}
		else if (IsUdp(data))
		{
			// udp
			protocol = L"Udp";
			info = GetUdpInfo(data);
		}
		else if (IsIcmp(data))
		{
			// icmp
			protocol = L"Icmp";
			info = GetIcmpInfo(data);
		}
		else
		{
			// ipv4
			protocol = L"Ipv4";
			info = GetIpv4Info(data);
		}
	}
	else if (IsIpv6(data))
	{
		// ipv6
		protocol = L"Ipv6";
		//info = L"Unsopport yet";
		info = GetEtherInfo(data);
	}
	else if (IsArp(data))
	{
		protocol = L"Arp";
		//info = L"Unsopport yet";
		info = GetArpInfo(data);
	}
	else
	{
		// ��̫��֡
		protocol = L"Eth";
		info = GetEtherInfo(data);
	}
	return std::make_pair(protocol, info);
}

std::pair<std::wstring, std::wstring> CapNetParse::GetSourceDestIp(const UCHAR* data)
{
	if (IsIpv4(data)) {
		IpHeader* iph = (IpHeader*)(data + 14);

		SOCKADDR_IN srcAddr, dstAddr = { 0 };
		srcAddr.sin_addr.s_addr = iph->srcAddr;
		dstAddr.sin_addr.s_addr = iph->dstAddr;

		CHAR strSaddr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(srcAddr.sin_addr), strSaddr, INET_ADDRSTRLEN);

		CHAR strDaddr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(dstAddr.sin_addr), strDaddr, INET_ADDRSTRLEN);

		CA2W ca2w1(strSaddr);
		CA2W ca2w2(strDaddr);
		return std::pair<std::wstring, std::wstring>(ca2w1, ca2w2);
	}
	else if (IsIpv6(data))
	{
		Ipv6Header* iph = (Ipv6Header*)(data + 14);
		CHAR strSaddr[INET6_ADDRSTRLEN] = { 0 };
		inet_ntop(AF_INET6, &iph->saddr, strSaddr, INET6_ADDRSTRLEN);

		CHAR strDaddr[INET6_ADDRSTRLEN] = { 0 };
		inet_ntop(AF_INET6, &iph->daddr, strDaddr, INET6_ADDRSTRLEN);

		CA2W ca2w1(strSaddr);
		CA2W ca2w2(strDaddr);
		return std::pair<std::wstring, std::wstring>(ca2w1, ca2w2);
	}

	return std::pair<std::wstring, std::wstring>();
}

std::wstring CapNetParse::GetTcpInfo(const UCHAR* data)
{
	TcpHeader* tcpHdr;
	tcpHdr = (TcpHeader*)(data + 14 + 20);
	USHORT sport = ntohs(tcpHdr->sourPort);
	USHORT dport = ntohs(tcpHdr->destPort);
	USHORT window = tcpHdr->windowSize;
	INT flags = tcpHdr->flags;

	CapNetOutStreamW owss;
	owss << L"Դ�˿�:" << sport
		<< L" Ŀ��˿�:" << dport
		<< L" ���ڴ�С:" << window
		<< L" ��־:(" << flags << L") ";

	if (flags & 0x08) owss << L"PSH ���ݴ���";
	else if (flags & 0x10) owss << L"ACK ��Ӧ";
	else if (flags & 0x02) owss << L"SYN ��������";
	else if (flags & 0x20) owss << L"URG �����¼�";
	else if (flags & 0x01) owss << L"FIN �ر�����";
	else if (flags & 0x04) owss << L"RST ��������";
	else owss << L"None δ֪";

	return owss;
}

std::wstring CapNetParse::GetUdpInfo(const UCHAR* data)
{
	UdpHeader* udpHdr;
	udpHdr = (UdpHeader*)(data + 14 + 20);

	USHORT sport = ntohs(udpHdr->sport);
	USHORT dport = ntohs(udpHdr->dport);
	USHORT dataLen = ntohs(udpHdr->dataLen);

	CapNetOutStreamW owss;
	owss << L"Դ�˿�:" << sport
		<< L" Ŀ��˿�:" << dport
		<< L" ��С:" << dataLen;
	return owss;
}

std::wstring CapNetParse::GetIcmpInfo(const UCHAR* data)
{
	IcmpHeader* icmpHdr;
	icmpHdr = (struct IcmpHeader*)(data + 14 + 20);

	INT type = icmpHdr->type;
	INT initTime = icmpHdr->initTime;
	INT sendTime = icmpHdr->sendTime;
	INT recvTime = icmpHdr->recvTime;
	CapNetOutStreamW owss;
	if (type == 8)
	{
		owss << L"����ʱ���:" << initTime
			<< L" ����ʱ���:" << sendTime
			<< L" ����ʱ���:" << recvTime
			<< L" ����:";

		switch (type)
		{
		case 0: owss << L"����Ӧ����"; break;
		case 8: owss << L"����������"; break;
		default:break;
		}
	}
	return owss;
}

std::wstring CapNetParse::GetIpv4Info(const UCHAR* data)
{
	IpHeader* iph;
	iph = (IpHeader*)(data + 14);
	SOCKADDR_IN srcAddr = { 0 }, dstAddr = { 0 };

	USHORT checkSum = ntohs(iph->checkSum);
	UINT ttl = iph->timeToLive;
	INT proto = iph->protocol;

	srcAddr.sin_addr.s_addr = iph->srcAddr;
	dstAddr.sin_addr.s_addr = iph->dstAddr;

	CapNetOutStreamW owss;

	owss << L"У���:" << std::hex << checkSum
		<< L" TTL:" << std::dec << ttl
		<< L" Э������:";
	switch (iph->protocol)
	{
	case 1: owss << L"ICMP"; break;
	case 2: owss << L"IGMP"; break;
	case 6: owss << L"TCP";  break;
	case 17: owss << L"UDP"; break;
	case 89: owss << L"OSPF"; break;
	default: owss << L"None"; break;
	}
	return owss;
}

std::wstring CapNetParse::GetEtherInfo(const UCHAR* data)
{
	EtherHeader* ethHdr;
	ethHdr = (EtherHeader*)data;

	USHORT etherType = ntohs(ethHdr->etherType);  // ��̫������
	u_char* etherSrc = ethHdr->etherSHost;         // ��̫��ԭʼMAC��ַ
	u_char* etherDst = ethHdr->etherDHost;         // ��̫��Ŀ��MAC��ַ

	CapNetOutStreamW owss;
	owss << L"����:0x" << std::hex << etherType
		<< CapNetOutStreamW::Format(L" ԭMAC��ַ: %02X:%02X:%02X:%02X:%02X:%02X ",
			etherSrc[0], etherSrc[1], etherSrc[2], etherSrc[3], etherSrc[4], etherSrc[5])
		<< CapNetOutStreamW::Format(L" Ŀ��MAC��ַ: %02X:%02X:%02X:%02X:%02X:%02X ",
			etherDst[0], etherDst[1], etherDst[2], etherDst[3], etherDst[4], etherDst[5]);
	return owss;
}

std::wstring CapNetParse::GetHttpInfo(const UCHAR* data)
{
	HttpHeader* httpProtocol = { 0 };
	// +14 ����������·�� +20 ����IP�� +20 ����TCP
	httpProtocol = (HttpHeader*)(data + 14 + 20 + 20);
	CapNetOutStreamW owss;
	owss << httpProtocol->url;
	return owss;
}

std::wstring CapNetParse::GetArpInfo(const UCHAR* data)
{
	ArpHeader* arpHdr;

	arpHdr = (struct ArpHeader*)(data + 14);

	USHORT hardwareType = ntohs(arpHdr->hardwareType);
	USHORT protocolType = ntohs(arpHdr->protocolType);
	INT hardwareLength = arpHdr->hardwareLength;
	INT protocolLength = arpHdr->protocolLength;
	USHORT operationCode = ntohs(arpHdr->operationCode);

	// �ж��Ƿ�ΪARP�����
	CapNetOutStreamW woss;
	if (hardwareLength == 6 && protocolLength == 4)
	{
		woss << L"ԭMAC��ַ:";
		for (INT x = 0; x < 6; x++)
			woss << woss.Format(L"%X:", arpHdr->sourceEthernetAddress[x]);

		woss << L" Ŀ��MAC��ַ:";
		for (INT x = 0; x < 6; x++)
			woss << woss.Format(L"%X:", arpHdr->destinationEthernetAddress[x]);

		switch (operationCode)
		{
		case 1: woss << L" ARP ����"; break;
		case 2: woss << L"ARP Ӧ�� "; break;
		case 3: woss << L"RARP ���� "; break;
		case 4: woss << L"RARP Ӧ�� "; break;
		default: break;
		}
	}
	return woss;
}

BOOL CapNetParse::CompareProtocolW(const UCHAR* data, const WCHAR* protocol)
{
	if (StrCmpW(protocol, L"eth") == 0)
	{
		return TRUE;
	}
	else if (StrCmpW(protocol, L"arp") == 0)
	{
		return IsArp(data);
	}
	else if (StrCmpW(protocol, L"ip") == 0)
	{
		return IsIpv4(data) || IsIpv6(data);
	}
	else if (StrCmpW(protocol, L"tcp") == 0)
	{
		return IsTcp(data);
	}
	else if (StrCmpW(protocol, L"udp") == 0)
	{
		return IsUdp(data);
	}
	else if (StrCmpW(protocol, L"icmp") == 0)
	{
		return IsIcmp(data);
	}
	else if (StrCmpW(protocol, L"http") == 0)
	{
		return IsHttp(data);
	}
	else if (StrCmpW(protocol, L"https") == 0)
	{
		return IsHttps(data);
	}
	return FALSE;
}

BOOL CapNetParse::CompareIpW(const UCHAR* data, const WCHAR* ip)
{
	auto res = GetSourceDestIp(data);
	return StrCmpW(res.first.c_str(), ip) == 0 || StrCmpW(res.second.c_str(), ip) == 0;
}
