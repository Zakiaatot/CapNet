#ifndef _CAPNET_PROTOCOL_H_
#define _CAPNET_PROTOCOL_H_
#include <pcap.h>

typedef struct EtherHeader {
	UCHAR etherDHost[6];    // 目标地址
	UCHAR etherSHost[6];    // 源地址
	USHORT etherType;       // 以太网类型
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
	SHORT sourPort;                 // 源端口号16bit
	SHORT destPort;                 // 目的端口号16bit
	UINT sequNum;           // 序列号32bit
	UINT cknowledgeNum;    // 确认号32bit
	UCHAR reserved : 4, offset : 4; // 预留偏移
	UCHAR  flags;               // 标志 
	SHORT windowSize;               // 窗口大小16bit
	SHORT checkSum;                 // 检验和16bit
	SHORT surgentPointer;           // 紧急数据偏移量16bit
}TcpHeader;


typedef struct UdpHeader {
	UINT sport;   // 源端口
	UINT dport;   // 目标端口
	UCHAR zero;     // 保留位
	UCHAR proto;    // 协议标识
	USHORT dataLen; // UDP数据长度
}UdpHeader;

typedef struct IcmpHeader {
	UCHAR type;        // ICMP类型
	UCHAR code;        // 代码
	USHORT checkSum;   // 校验和
	USHORT identification; // 标识
	USHORT sequence;       // 序列号
	UINT initTime;      // 发起时间戳
	USHORT recvTime;      // 接受时间戳
	USHORT sendTime;      // 传输时间戳
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

