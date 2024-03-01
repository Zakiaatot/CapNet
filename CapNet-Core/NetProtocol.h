#ifndef _NET_PROTOCOL_H_
#define _NET_PROTOCOL_H_
#include <pcap.h>

//Mac头部，总长度14字节  
typedef struct _eth_hdr
{
	unsigned char dstmac[6]; //目标mac地址  
	unsigned char srcmac[6]; //源mac地址  
	unsigned short eth_type; //以太网类型  
}eth_hdr;

/* 4 bytes IP address */
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

/* IPv4 header */
typedef struct ip_header
{
	u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
	u_char	tos;			// Type of service 
	u_short tlen;			// Total length 
	u_short identification; // Identification
	u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)
	u_char	ttl;			// Time to live
	u_char	proto;			// Protocol
	u_short crc;			// Header checksum
	ip_address	saddr;		// Source address
	ip_address	daddr;		// Destination address
	u_int	op_pad;			// Option + Padding
}ip_header;

/* IPv6 header */
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

// tcp
typedef struct {
	uint16_t src_port;
	uint16_t dst_port;
	uint32_t seq;
	uint32_t ack;
	uint8_t  data_offset;  // 4 bits
	uint8_t  flags;
	uint16_t window_size;
	uint16_t checksum;
	uint16_t urgent_p;
} tcp_header;

#endif // !_NET_PROTOCOL_H_

