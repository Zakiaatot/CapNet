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
		short SourPort;                 // Դ�˿ں�16bit
		short DestPort;                 // Ŀ�Ķ˿ں�16bit
		unsigned int SequNum;           // ���к�32bit
		unsigned int AcknowledgeNum;    // ȷ�Ϻ�32bit
		unsigned char reserved : 4, offset : 4; // Ԥ��ƫ��

		unsigned char  flags;               // ��־ 

		short WindowSize;               // ���ڴ�С16bit
		short CheckSum;                 // �����16bit
		short surgentPointer;           // ��������ƫ����16bit
	}tcp_header;

	struct tcp_header* tcp_protocol;
	// +14 ����������·�� +20 ����IP��
	tcp_protocol = (struct tcp_header*)(data + 14 + 20);

	u_short sport = ntohs(tcp_protocol->SourPort);
	u_short dport = ntohs(tcp_protocol->DestPort);
	int window = tcp_protocol->WindowSize;
	int flags = tcp_protocol->flags;

	WCHAR buf[256] = { 0 };
	wsprintf(buf, L"Դ�˿�: %6d --> Ŀ��˿�: %6d --> ���ڴ�С: %7d --> ��־: (%d)",
		sport, dport, window, flags);

	WCHAR* p = buf + wcslen(buf);

	if (flags & 0x08) wsprintf(p, L"PSH ���ݴ���");
	else if (flags & 0x10) wsprintf(p, L"ACK ��Ӧ");
	else if (flags & 0x02) wsprintf(p, L"SYN ��������");
	else if (flags & 0x20) wsprintf(p, L"URG ");
	else if (flags & 0x01) wsprintf(p, L"FIN �ر�����");
	else if (flags & 0x04) wsprintf(p, L"RST ��������");
	else wsprintf(p, L"None δ֪");

	return buf;
}

std::wstring Parse::GetUdpInfo(const UCHAR* data)
{
	typedef struct udp_header {
		uint32_t sport;   // Դ�˿�
		uint32_t dport;   // Ŀ��˿�
		uint8_t zero;     // ����λ
		uint8_t proto;    // Э���ʶ
		uint16_t datalen; // UDP���ݳ���
	}udp_header;

	struct udp_header* udp_protocol;
	// +14 ����������·�� +20 ����IP��
	udp_protocol = (struct udp_header*)(data + 14 + 20);

	u_short sport = ntohs(udp_protocol->sport);
	u_short dport = ntohs(udp_protocol->dport);
	u_short datalen = ntohs(udp_protocol->datalen);

	WCHAR buf[256] = { 0 };
	wsprintf(buf, L"Դ�˿�: %5d --> Ŀ��˿�: %5d --> ��С: %5d \n", sport, dport, datalen);
	return buf;
}

std::wstring Parse::GetIcmpInfo(const UCHAR* data)
{
	typedef struct icmp_header {
		uint8_t type;        // ICMP����
		uint8_t code;        // ����
		uint16_t checksum;   // У���
		uint16_t identification; // ��ʶ
		uint16_t sequence;       // ���к�
		uint32_t init_time;      // ����ʱ���
		uint16_t recv_time;      // ����ʱ���
		uint16_t send_time;      // ����ʱ���
	}icmp_header;

	struct icmp_header* icmp_protocol;
	// +14 ����������·�� +20 ����IP��
	icmp_protocol = (struct icmp_header*)(data + 14 + 20);

	int type = icmp_protocol->type;
	int init_time = icmp_protocol->init_time;
	int send_time = icmp_protocol->send_time;
	int recv_time = icmp_protocol->recv_time;
	WCHAR buf[256] = { 0 };
	if (type == 8)
	{
		wsprintf(buf, L"����ʱ���: %d --> ����ʱ���: %d --> ����ʱ���: %d ����: ",
			init_time, send_time, recv_time);
		WCHAR* p = buf + wcslen(buf);
		switch (type)
		{
		case 0: wsprintf(p, L"����Ӧ���� \n"); break;
		case 8: wsprintf(p, L"���������� \n"); break;
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

	// +14 ����������·��
	ip_protocol = (struct ip_header*)(data + 14);
	SOCKADDR_IN Src_Addr = { 0 }, Dst_Addr = { 0 };

	u_short check_sum = ntohs(ip_protocol->check_sum);
	int ttl = ip_protocol->time_to_live;
	int proto = ip_protocol->Protocol;

	Src_Addr.sin_addr.s_addr = ip_protocol->SrcAddr;
	Dst_Addr.sin_addr.s_addr = ip_protocol->DstAddr;

	WCHAR buf[256] = { 0 };


	wsprintf(buf, L"У���: %5X --> TTL: %4d --> Э������: ", check_sum, ttl);
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
