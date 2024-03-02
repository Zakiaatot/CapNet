#ifndef _CAPNET_PROTOCOL_H_
#define _CAPNET_PROTOCOL_H_
#include <pcap.h>

typedef struct ether_header {
	u_char ether_dhost[6];    // Ŀ���ַ
	u_char ether_shost[6];    // Դ��ַ
	u_short ether_type;       // ��̫������
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


typedef struct udp_header {
	uint32_t sport;   // Դ�˿�
	uint32_t dport;   // Ŀ��˿�
	uint8_t zero;     // ����λ
	uint8_t proto;    // Э���ʶ
	uint16_t datalen; // UDP���ݳ���
}udp_header;

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

#endif // !_CAPNET_PROTOCOL_H_

