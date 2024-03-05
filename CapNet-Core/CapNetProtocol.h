#ifndef _CAPNET_PROTOCOL_H_
#define _CAPNET_PROTOCOL_H_
#include <pcap.h>

typedef struct EtherHeader {
	UCHAR etherDHost[6];    // Ŀ���ַ
	UCHAR etherSHost[6];    // Դ��ַ
	USHORT etherType;       // ��̫������
} EtherHeader;


typedef struct IpHeader
{
	CHAR version : 4;
	CHAR headerLength : 4;
	CHAR tos;
	USHORT totlaLength;
	USHORT identification;
	USHORT flagsOffset;
	CHAR timeToLive;
	CHAR protocol;
	USHORT checkSum;
	UINT srcAddr;
	UINT dstAddr;
}IpHeader;

typedef struct Ipv6Header
{
	UINT
		version : 4,
		trafficClass : 8,
		flowLabel : 20;
	USHORT length;
	UCHAR  nextHeader;
	UCHAR  hopLimit;
	struct in6_addr saddr;
	struct in6_addr daddr;
} Ipv6Header;


typedef struct TcpHeader
{
	SHORT sourPort;                 // Դ�˿ں�16bit
	SHORT destPort;                 // Ŀ�Ķ˿ں�16bit
	UINT sequNum;           // ���к�32bit
	UINT cknowledgeNum;    // ȷ�Ϻ�32bit
	UCHAR reserved : 4, offset : 4; // Ԥ��ƫ��
	UCHAR  flags;               // ��־ 
	SHORT windowSize;               // ���ڴ�С16bit
	SHORT checkSum;                 // �����16bit
	SHORT surgentPointer;           // ��������ƫ����16bit
}TcpHeader;


typedef struct UdpHeader {
	UINT sport;   // Դ�˿�
	UINT dport;   // Ŀ��˿�
	UCHAR zero;     // ����λ
	UCHAR proto;    // Э���ʶ
	USHORT dataLen; // UDP���ݳ���
}UdpHeader;

typedef struct IcmpHeader {
	UCHAR type;        // ICMP����
	UCHAR code;        // ����
	USHORT checkSum;   // У���
	USHORT identification; // ��ʶ
	USHORT sequence;       // ���к�
	UINT initTime;      // ����ʱ���
	USHORT recvTime;      // ����ʱ���
	USHORT sendTime;      // ����ʱ���
}IcmpHeader;

typedef struct HttpHeader
{
	CHAR url[512];
}HttpHeader;

typedef struct ArpHeader
{
	USHORT hardwareType;
	USHORT protocolType;
	UCHAR hardwareLength;
	UCHAR protocolLength;
	USHORT operationCode;
	UCHAR sourceEthernetAddress[6];
	UCHAR sourceIpAddress[4];
	UCHAR destinationEthernetAddress[6];
	UCHAR destinationIpAddress[4];
}ArpHeader;

#endif // !_CAPNET_PROTOCOL_H_

