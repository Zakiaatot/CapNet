#include <atlstr.h>
#include <vector>
#include <ws2tcpip.h>
#include "CapNetParse.h"
#include "CapNetProtocol.h"
#include "CapNetStream.h"

#pragma comment(lib,"Ws2_32.lib")

BOOL CapNetParse::IsIpv6(const UCHAR* data)
{
	ether_header* ethHdr = (ether_header*)data;
	USHORT ethType = ntohs(ethHdr->ether_type);
	if (ethType == 0x86DD)
		return TRUE;
	else
		return FALSE;
}

BOOL CapNetParse::IsIpv4(const UCHAR* data)
{
	ether_header* ethHdr = (ether_header*)data;
	USHORT ethType = ntohs(ethHdr->ether_type);
	if (ethType == 0x0800)
		return TRUE;
	else
		return FALSE;
}

BOOL CapNetParse::IsUdp(const UCHAR* data)
{
	if (IsIpv4(data))
	{
		ip_header* iph = (ip_header*)(data + 14);
		return iph->Protocol == 0x11;
	}
	if (IsIpv6(data))
	{
		ipv6_header* iph = (ipv6_header*)(data + 14);
		return iph->next_header == 0x11;
	}
	return FALSE;
}

BOOL CapNetParse::IsTcp(const UCHAR* data)
{
	if (IsIpv4(data))
	{
		ip_header* iph = (ip_header*)(data + 14);
		return iph->Protocol == 0x06;
	}
	if (IsIpv6(data))
	{
		ipv6_header* iph = (ipv6_header*)(data + 14);
		return iph->next_header == 0x06;
	}
	return FALSE;
}

BOOL CapNetParse::IsIcmp(const UCHAR* data)
{
	if (IsIpv4(data))
	{
		ip_header* iph = (ip_header*)(data + 14);
		return iph->Protocol == 0x01;
	}
	if (IsIpv6(data))
	{
		ipv6_header* iph = (ipv6_header*)(data + 14);
		return iph->next_header == 0x01;
	}
	return FALSE;
}

BOOL CapNetParse::IsArp(const UCHAR* data)
{
	ether_header* ethHdr = (ether_header*)data;
	USHORT ethType = ntohs(ethHdr->ether_type);
	if (ethType == 0x0806)
		return TRUE;
	else
		return FALSE;
}

BOOL CapNetParse::IsHttp(const UCHAR* data)
{
	if (!IsTcp(data))
		return FALSE;
	const tcp_header* tcpHeader = (tcp_header*)(data + 14 + 20);
	if (ntohs(tcpHeader->DestPort) == 80 || ntohs(tcpHeader->SourPort) == 80)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CapNetParse::IsHttps(const UCHAR* data)
{
	if (!IsTcp(data))
		return FALSE;
	const tcp_header* tcpHeader = (tcp_header*)(data + 14 + 20);
	if (ntohs(tcpHeader->DestPort) == 443 || ntohs(tcpHeader->SourPort) == 443)
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
				info = L"To do";
			}
			else if (IsHttps(data))
			{
				// https
				protocol = L"Https";
				info = L"To do";
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
		info = L"Unsopport yet";
	}
	else if (IsArp(data))
	{
		protocol = L"Arp";
		info = L"Unsopport yet";
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
		ip_header* iph = (ip_header*)(data + 14);

		SOCKADDR_IN Src_Addr, Dst_Addr = { 0 };
		Src_Addr.sin_addr.s_addr = iph->SrcAddr;
		Dst_Addr.sin_addr.s_addr = iph->DstAddr;

		CHAR strSaddr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(Src_Addr.sin_addr), strSaddr, INET_ADDRSTRLEN);

		CHAR strDaddr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(Dst_Addr.sin_addr), strDaddr, INET_ADDRSTRLEN);

		CA2W ca2w1(strSaddr);
		CA2W ca2w2(strDaddr);
		return std::pair<std::wstring, std::wstring>(ca2w1, ca2w2);
	}
	else if (IsIpv6(data))
	{
		ipv6_header* iph = (ipv6_header*)(data + 14);
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
	struct tcp_header* tcp_protocol;
	// +14 跳过数据链路层 +20 跳过IP层
	tcp_protocol = (struct tcp_header*)(data + 14 + 20);

	USHORT sport = ntohs(tcp_protocol->SourPort);
	USHORT dport = ntohs(tcp_protocol->DestPort);
	USHORT window = tcp_protocol->WindowSize;
	int flags = tcp_protocol->flags;

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
	struct udp_header* udp_protocol;
	// +14 跳过数据链路层 +20 跳过IP层
	udp_protocol = (struct udp_header*)(data + 14 + 20);

	u_short sport = ntohs(udp_protocol->sport);
	u_short dport = ntohs(udp_protocol->dport);
	u_short datalen = ntohs(udp_protocol->datalen);

	CapNetOutStreamW owss;
	owss << L"源端口:" << sport
		<< L" 目标端口:" << dport
		<< L" 大小:" << datalen;
	return owss;
}

std::wstring CapNetParse::GetIcmpInfo(const UCHAR* data)
{
	struct icmp_header* icmp_protocol;
	// +14 跳过数据链路层 +20 跳过IP层
	icmp_protocol = (struct icmp_header*)(data + 14 + 20);

	int type = icmp_protocol->type;
	int init_time = icmp_protocol->init_time;
	int send_time = icmp_protocol->send_time;
	int recv_time = icmp_protocol->recv_time;
	CapNetOutStreamW owss;
	if (type == 8)
	{
		owss << L"发起时间戳:" << init_time
			<< L" 传输时间戳:" << send_time
			<< L" 接收时间戳:" << recv_time
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
	struct ip_header* ip_protocol;

	// +14 跳过数据链路层
	ip_protocol = (struct ip_header*)(data + 14);
	SOCKADDR_IN Src_Addr = { 0 }, Dst_Addr = { 0 };

	u_short check_sum = ntohs(ip_protocol->check_sum);
	UINT ttl = ip_protocol->time_to_live;
	int proto = ip_protocol->Protocol;

	Src_Addr.sin_addr.s_addr = ip_protocol->SrcAddr;
	Dst_Addr.sin_addr.s_addr = ip_protocol->DstAddr;

	CapNetOutStreamW owss;


	owss << L"校验和:" << std::hex << check_sum
		<< L" TTL:" << std::dec << ttl
		<< L" 协议类型:";
	switch (ip_protocol->Protocol)
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
	struct ether_header* eth_protocol;
	eth_protocol = (struct ether_header*)data;

	u_short ether_type = ntohs(eth_protocol->ether_type);  // 以太网类型
	u_char* ether_src = eth_protocol->ether_shost;         // 以太网原始MAC地址
	u_char* ether_dst = eth_protocol->ether_dhost;         // 以太网目标MAC地址

	CapNetOutStreamW owss;
	owss << L"类型:0x" << std::hex << ether_type
		<< CapNetOutStreamW::Format(L" 原MAC地址: %02X:%02X:%02X:%02X:%02X:%02X ",
			ether_src[0], ether_src[1], ether_src[2], ether_src[3], ether_src[4], ether_src[5])
		<< CapNetOutStreamW::Format(L" 目标MAC地址: %02X:%02X:%02X:%02X:%02X:%02X ",
			ether_dst[0], ether_dst[1], ether_dst[2], ether_dst[3], ether_dst[4], ether_dst[5]);
	return owss;
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
