#include <atlstr.h>
#include <ws2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")

#include "Parse.h"
#include "NetProtocol.h"

BOOL Parse::IsIpv6(const UCHAR* data)
{
	eth_hdr* ethHdr = (eth_hdr*)data;
	USHORT ethType = ntohs(ethHdr->eth_type);
	if (ethType == 0x86DD)
		return TRUE;
	else
		return FALSE;
}

BOOL Parse::IsIpv4(const UCHAR* data)
{
	eth_hdr* ethHdr = (eth_hdr*)data;
	USHORT ethType = ntohs(ethHdr->eth_type);
	if (ethType == 0x0800)
		return TRUE;
	else
		return FALSE;
}

BOOL Parse::IsUdp(const UCHAR* data)
{
	if (IsIpv4(data))
	{
		ip_header* iph = (ip_header*)(data + 14);
		return iph->tos == 0x11;
	}
	if (IsIpv6(data))
	{
		ipv6_header* iph = (ipv6_header*)(data + 14);
		return iph->next_header == 0x11;
	}
	return FALSE;
}

BOOL Parse::IsTcp(const UCHAR* data)
{
	if (IsIpv4(data))
	{
		ip_header* iph = (ip_header*)(data + 14);
		return iph->tos == 0x06;
	}
	if (IsIpv6(data))
	{
		ipv6_header* iph = (ipv6_header*)(data + 14);
		return iph->next_header == 0x06;
	}
	return FALSE;
}

BOOL Parse::IsIcmp(const UCHAR* data)
{
	if (IsIpv4(data))
	{
		ip_header* iph = (ip_header*)(data + 14);
		return iph->tos == 0x01;
	}
	if (IsIpv6(data))
	{
		ipv6_header* iph = (ipv6_header*)(data + 14);
		return iph->next_header == 0x01;
	}
	return FALSE;
}

BOOL Parse::IsArp(const UCHAR* data)
{
	eth_hdr* ethHdr = (eth_hdr*)data;
	USHORT ethType = ntohs(ethHdr->eth_type);
	if (ethType == 0x0806)
		return TRUE;
	else
		return FALSE;
}

BOOL Parse::IsHttp(const UCHAR* data)
{
	if (!IsTcp(data))
		return FALSE;
	const tcp_header* tcpHeader = (tcp_header*)data;
	if (tcpHeader->dst_port == 80 || tcpHeader->src_port == 80)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL Parse::IsHttps(const UCHAR* data)
{
	if (!IsTcp(data))
		return FALSE;
	const tcp_header* tcpHeader = (tcp_header*)data;
	if (tcpHeader->dst_port == 443 || tcpHeader->src_port == 443)
	{
		return TRUE;
	}
	return FALSE;
}

std::pair<std::wstring, std::wstring> Parse::GetProtocolAndInfo(const UCHAR* data)
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
			protocol = L"Tcp";
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
		protocol = L"Unknown";
		info = L"Unknown";
	}
	return std::make_pair(protocol, info);
}

std::pair<std::wstring, std::wstring> Parse::GetSourceDestIp(const UCHAR* data)
{

	if (IsIpv4(data)) {
		ip_header* iph = (ip_header*)(data + 14);
		CHAR strSaddr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(iph->saddr), strSaddr, INET_ADDRSTRLEN);

		CHAR strDaddr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(iph->daddr), strDaddr, INET_ADDRSTRLEN);

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

std::wstring Parse::GetTcpInfo(const UCHAR* data)
{
	typedef struct tcp_header
	{
		short SourPort;                 // 源端口号16bit
		short DestPort;                 // 目的端口号16bit
		unsigned int SequNum;           // 序列号32bit
		unsigned int AcknowledgeNum;    // 确认号32bit
		unsigned char reserved : 4, offset : 4; // 预留偏移

		unsigned char  flags;               // 标志 

		short WindowSize;               // 窗口大小16bit
		short CheckSum;                 // 检验和16bit
		short surgentPointer;           // 紧急数据偏移量16bit
	}tcp_header;

	struct tcp_header* tcp_protocol;
	// +14 跳过数据链路层 +20 跳过IP层
	tcp_protocol = (struct tcp_header*)(data + 14 + 20);

	u_short sport = ntohs(tcp_protocol->SourPort);
	u_short dport = ntohs(tcp_protocol->DestPort);
	int window = tcp_protocol->WindowSize;
	int flags = tcp_protocol->flags;

	WCHAR buf[256] = { 0 };
	wsprintf(buf, L"源端口: %6d --> 目标端口: %6d --> 窗口大小: %7d --> 标志: (%d)",
		sport, dport, window, flags);

	WCHAR* p = buf + wcslen(buf);

	if (flags & 0x08) wsprintf(p, L"PSH 数据传输");
	else if (flags & 0x10) wsprintf(p, L"ACK 响应");
	else if (flags & 0x02) wsprintf(p, L"SYN 建立连接");
	else if (flags & 0x20) wsprintf(p, L"URG ");
	else if (flags & 0x01) wsprintf(p, L"FIN 关闭连接");
	else if (flags & 0x04) wsprintf(p, L"RST 连接重置");
	else wsprintf(p, L"None 未知");

	return buf;
}

std::wstring Parse::GetUdpInfo(const UCHAR* data)
{
	typedef struct udp_header {
		uint32_t sport;   // 源端口
		uint32_t dport;   // 目标端口
		uint8_t zero;     // 保留位
		uint8_t proto;    // 协议标识
		uint16_t datalen; // UDP数据长度
	}udp_header;

	struct udp_header* udp_protocol;
	// +14 跳过数据链路层 +20 跳过IP层
	udp_protocol = (struct udp_header*)(data + 14 + 20);

	u_short sport = ntohs(udp_protocol->sport);
	u_short dport = ntohs(udp_protocol->dport);
	u_short datalen = ntohs(udp_protocol->datalen);

	WCHAR buf[256] = { 0 };
	wsprintf(buf, L"源端口: %5d --> 目标端口: %5d --> 大小: %5d \n", sport, dport, datalen);
	return buf;
}

std::wstring Parse::GetIcmpInfo(const UCHAR* data)
{
	typedef struct icmp_header {
		uint8_t type;        // ICMP类型
		uint8_t code;        // 代码
		uint16_t checksum;   // 校验和
		uint16_t identification; // 标识
		uint16_t sequence;       // 序列号
		uint32_t init_time;      // 发起时间戳
		uint16_t recv_time;      // 接受时间戳
		uint16_t send_time;      // 传输时间戳
	}icmp_header;

	struct icmp_header* icmp_protocol;
	// +14 跳过数据链路层 +20 跳过IP层
	icmp_protocol = (struct icmp_header*)(data + 14 + 20);

	int type = icmp_protocol->type;
	int init_time = icmp_protocol->init_time;
	int send_time = icmp_protocol->send_time;
	int recv_time = icmp_protocol->recv_time;
	WCHAR buf[256] = { 0 };
	if (type == 8)
	{
		wsprintf(buf, L"发起时间戳: %d --> 传输时间戳: %d --> 接收时间戳: %d 方向: ",
			init_time, send_time, recv_time);
		WCHAR* p = buf + wcslen(buf);
		switch (type)
		{
		case 0: wsprintf(p, L"回显应答报文 \n"); break;
		case 8: wsprintf(p, L"回显请求报文 \n"); break;
		default:break;
		}
	}
	return buf;
}

std::wstring Parse::GetIpv4Info(const UCHAR* data)
{
	typedef struct ip_header
	{
		char version : 4;
		char headerlength : 4;
		char cTOS;
		unsigned short totla_length;
		unsigned short identification;
		unsigned short flags_offset;
		char time_to_live;
		char Protocol;
		unsigned short check_sum;
		unsigned int SrcAddr;
		unsigned int DstAddr;
	}ip_header;

	struct ip_header* ip_protocol;

	// +14 跳过数据链路层
	ip_protocol = (struct ip_header*)(data + 14);
	SOCKADDR_IN Src_Addr = { 0 }, Dst_Addr = { 0 };

	u_short check_sum = ntohs(ip_protocol->check_sum);
	int ttl = ip_protocol->time_to_live;
	int proto = ip_protocol->Protocol;

	Src_Addr.sin_addr.s_addr = ip_protocol->SrcAddr;
	Dst_Addr.sin_addr.s_addr = ip_protocol->DstAddr;

	WCHAR buf[256] = { 0 };


	wsprintf(buf, L"校验和: %5X --> TTL: %4d --> 协议类型: ", check_sum, ttl);
	WCHAR* p = buf + wcslen(buf);
	switch (ip_protocol->Protocol)
	{
	case 1: wsprintf(p, L"ICMP \n"); break;
	case 2: wsprintf(p, L"IGMP \n"); break;
	case 6: wsprintf(p, L"TCP \n");  break;
	case 17: wsprintf(p, L"UDP \n"); break;
	case 89: wsprintf(p, L"OSPF \n"); break;
	default: wsprintf(p, L"None \n"); break;
	}
	return buf;
}
