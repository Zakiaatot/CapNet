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
		// 以太网帧
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
	owss << L"源端口:" << sport
		<< L" 目标端口:" << dport
		<< L" 窗口大小:" << window
		<< L" 标志:(" << flags << L") ";

	if (flags & 0x08) owss << L"PSH 数据传输";
	else if (flags & 0x10) owss << L"ACK 响应";
	else if (flags & 0x02) owss << L"SYN 建立连接";
	else if (flags & 0x20) owss << L"URG 紧急事件";
	else if (flags & 0x01) owss << L"FIN 关闭连接";
	else if (flags & 0x04) owss << L"RST 连接重置";
	else owss << L"None 未知";

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
	owss << L"源端口:" << sport
		<< L" 目标端口:" << dport
		<< L" 大小:" << dataLen;
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
		owss << L"发起时间戳:" << initTime
			<< L" 传输时间戳:" << sendTime
			<< L" 接收时间戳:" << recvTime
			<< L" 方向:";

		switch (type)
		{
		case 0: owss << L"回显应答报文"; break;
		case 8: owss << L"回显请求报文"; break;
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

	owss << L"校验和:" << std::hex << checkSum
		<< L" TTL:" << std::dec << ttl
		<< L" 协议类型:";
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

	USHORT etherType = ntohs(ethHdr->etherType);  // 以太网类型
	u_char* etherSrc = ethHdr->etherSHost;         // 以太网原始MAC地址
	u_char* etherDst = ethHdr->etherDHost;         // 以太网目标MAC地址

	CapNetOutStreamW owss;
	owss << L"类型:0x" << std::hex << etherType
		<< CapNetOutStreamW::Format(L" 原MAC地址: %02X:%02X:%02X:%02X:%02X:%02X ",
			etherSrc[0], etherSrc[1], etherSrc[2], etherSrc[3], etherSrc[4], etherSrc[5])
		<< CapNetOutStreamW::Format(L" 目标MAC地址: %02X:%02X:%02X:%02X:%02X:%02X ",
			etherDst[0], etherDst[1], etherDst[2], etherDst[3], etherDst[4], etherDst[5]);
	return owss;
}

std::wstring CapNetParse::GetHttpInfo(const UCHAR* data)
{
	HttpHeader* httpProtocol = { 0 };
	// +14 跳过数据链路层 +20 跳过IP层 +20 跳过TCP
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

	// 判读是否为ARP请求包
	CapNetOutStreamW woss;
	if (hardwareLength == 6 && protocolLength == 4)
	{
		woss << L"原MAC地址:";
		for (INT x = 0; x < 6; x++)
			woss << woss.Format(L"%X:", arpHdr->sourceEthernetAddress[x]);

		woss << L" 目标MAC地址:";
		for (INT x = 0; x < 6; x++)
			woss << woss.Format(L"%X:", arpHdr->destinationEthernetAddress[x]);

		switch (operationCode)
		{
		case 1: woss << L" ARP 请求"; break;
		case 2: woss << L"ARP 应答 "; break;
		case 3: woss << L"RARP 请求 "; break;
		case 4: woss << L"RARP 应答 "; break;
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
