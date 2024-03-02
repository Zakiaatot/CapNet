#ifndef _CAPNET_PROTOCOL_H_
#define _CAPNET_PROTOCOL_H_
#include <pcap.h>

typedef struct ether_header {
	u_char ether_dhost[6];    // 目标地址
	u_char ether_shost[6];    // 源地址
	u_short ether_type;       // 以太网类型
} ether_header;


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

typedef struct ipv6_header
{
	unsigned int
		version : 4,
		traffic_class : 8,
		flow_label : 20;
	uint16_t length;
	uint8_t  next_header;
	uint8_t  hop_limit;
	struct in6_addr saddr;
	struct in6_addr daddr;
} ipv6_header;


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


typedef struct udp_header {
	uint32_t sport;   // 源端口
	uint32_t dport;   // 目标端口
	uint8_t zero;     // 保留位
	uint8_t proto;    // 协议标识
	uint16_t datalen; // UDP数据长度
}udp_header;

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

#endif // !_CAPNET_PROTOCOL_H_

