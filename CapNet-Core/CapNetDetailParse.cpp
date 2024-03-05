#include <vector>
#include "CapNetProtocol.h"
#include "CapNetParse.h"
#include "CapNetDetailParse.h"
#include "CapNetStream.h"

using namespace CapNetParse;
using namespace CapNetDetailParse;

static WCHAR* NewWStr(const WCHAR* str)
{
	INT len = (INT)wcslen(str) + 1;
	WCHAR* buf = new WCHAR[len];
	memcpy(buf, str, len * sizeof(WCHAR));
	return buf;
}

static inline WCHAR* DelWstr(WCHAR* str)
{
	delete[] str;
}

static PTreeNode GenAttributes
(
	PTreeNode root,
	std::vector<std::wstring>& attributes
)
{
	if (!root || root->firstChild)
		return NULL;

	PTreeNode p = root;
	p->firstChild = new TreeNode{ 0 };
	p = p->firstChild;
	p->parent = root;
	p->text = NewWStr((*attributes.begin()).c_str());

	for (auto i = (attributes.begin() + 1); i != attributes.end(); i++)
	{
		p->firstBrother = new TreeNode{ 0 };
		p = p->firstBrother;
		p->parent = root;
		p->text = NewWStr((*i).c_str());
	}
	return p;
}

namespace CapNetDetailParse {
	Tree CreataDetailTree(const UCHAR* data)
	{
		return GenEtherInfo(data);
	}

	VOID DestroyDetailTree(Tree& tree)
	{
		if (!tree)
			return;
		if (tree->firstChild)
			DestroyDetailTree(tree->firstChild);
		if (tree->firstBrother)
			DestroyDetailTree(tree->firstBrother);
		DelWstr(tree->text);
		delete tree;
		tree = NULL;
	}

	PTreeNode GenEtherInfo(const UCHAR* data)
	{
		PTreeNode etherNode = new TreeNode{ 0 };
		etherNode->text = NewWStr(L"Ether");

		EtherHeader* ethHdr;
		ethHdr = (EtherHeader*)data;

		USHORT etherType = ntohs(ethHdr->etherType);  // 以太网类型
		UCHAR* etherSrc = ethHdr->etherSHost;         // 以太网原始MAC地址
		UCHAR* etherDst = ethHdr->etherDHost;         // 以太网目标MAC地址

		std::vector<std::wstring> attributes;
		CapNetOutStreamW srcOwss;
		CapNetOutStreamW dstOwss;
		CapNetOutStreamW typeOwss;

		srcOwss << srcOwss.Format(L"源MAC地址: %02X:%02X:%02X:%02X:%02X:%02X", etherSrc[0], etherSrc[1], etherSrc[2], etherSrc[3], etherSrc[4], etherSrc[5]);
		dstOwss << dstOwss.Format(L"目的MAC地址: %02X:%02X:%02X:%02X:%02X:%02X", etherDst[0], etherDst[1], etherDst[2], etherDst[3], etherDst[4], etherDst[5]);
		typeOwss << typeOwss.Format(L"类型: %04X", etherType);

		attributes.push_back(srcOwss);
		attributes.push_back(dstOwss);
		attributes.push_back(typeOwss);

		PTreeNode last = GenAttributes(etherNode, attributes);

		if (IsIpv4(data))
		{
			last->firstBrother = GenIpv4Info(data);
		}
		else if (IsIpv6(data))
		{
			last->firstBrother = GenIpv6Info(data);
		}
		else if (IsArp(data))
		{
			last->firstBrother = GenArpInfo(data);
		}
		return etherNode;
	}

	PTreeNode GenIpv4Info(const UCHAR* data)
	{
		PTreeNode ipv4Node = new TreeNode{ 0 };
		ipv4Node->text = NewWStr(L"Ipv4");

		IpHeader* iph;
		iph = (IpHeader*)(data + 14);
		SOCKADDR_IN srcAddr = { 0 }, dstAddr = { 0 };

		USHORT version = (iph->version) >> 4;
		USHORT hdrLen = (iph->headerLength & 0x0f) << 2;
		USHORT priority = (iph->tos) >> 5;
		USHORT service = (iph->tos >> 1) & 0x0f;
		USHORT totalLen = ntohs(iph->totlaLength);
		USHORT iden = ntohs(iph->identification);
		USHORT df = (ntohs(iph->flagsOffset) >> 14) & 0x01;
		USHORT mf = (ntohs(iph->flagsOffset) >> 13) & 0x01;
		USHORT offset = ntohs(iph->flagsOffset) & 0x1fff;
		USHORT checkSum = ntohs(iph->checkSum);
		UINT ttl = iph->timeToLive;
		INT proto = iph->protocol;

		srcAddr.sin_addr.s_addr = iph->srcAddr;
		dstAddr.sin_addr.s_addr = iph->dstAddr;

		CHAR strSaddr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(srcAddr.sin_addr), strSaddr, INET_ADDRSTRLEN);

		CHAR strDaddr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(dstAddr.sin_addr), strDaddr, INET_ADDRSTRLEN);

		std::vector<std::wstring> attributes;
		CapNetOutStreamW verOwss;
		CapNetOutStreamW hdrLenOwss;
		CapNetOutStreamW serviceOwss;
		CapNetOutStreamW totalLenOwss;
		CapNetOutStreamW idenOwss;
		CapNetOutStreamW flagOwss;
		CapNetOutStreamW offsetOwss;
		CapNetOutStreamW liveOwss;
		CapNetOutStreamW protocolOwss;
		CapNetOutStreamW checkSumOwss;
		CapNetOutStreamW srcOwss;
		CapNetOutStreamW dstOwss;

		verOwss << "协议版本: " << version;
		hdrLenOwss << "首部长度: " << hdrLen;
		serviceOwss << "服务类型: Priority=" << priority << " Service=" << service;
		totalLenOwss << "总长度: " << totalLen;
		idenOwss << "标识: " << iden;
		flagOwss << "标志位: DF=" << df << " MF=" << mf;
		offsetOwss << "片偏移: " << offset;
		liveOwss << "生存周期: " << ttl;
		protocolOwss << "协议类型: " << proto;
		checkSumOwss << "首部校验和: " << checkSum;
		srcOwss << "源地址: " << strSaddr;
		dstOwss << "目的地址: " << strDaddr;

		attributes.push_back(srcOwss);
		attributes.push_back(dstOwss);
		attributes.push_back(verOwss);
		attributes.push_back(hdrLenOwss);
		attributes.push_back(serviceOwss);
		attributes.push_back(totalLenOwss);
		attributes.push_back(idenOwss);
		attributes.push_back(flagOwss);
		attributes.push_back(offsetOwss);
		attributes.push_back(liveOwss);
		attributes.push_back(protocolOwss);
		attributes.push_back(checkSumOwss);

		PTreeNode last = GenAttributes(ipv4Node, attributes);

		if (IsTcp(data))
		{
			last->firstBrother = GenTcpInfo(data);
		}
		else if (IsUdp(data))
		{
			last->firstBrother = GenUdpInfo(data);
		}
		else if (IsIcmp(data))
		{
			last->firstBrother = GenIcmpInfo(data);
		}
		return ipv4Node;
	}

	PTreeNode GenIpv6Info(const UCHAR* data)
	{
		PTreeNode ipv6Node = new TreeNode{ 0 };
		ipv6Node->text = NewWStr(L"Ipv6");

		Ipv6Header* iph = (Ipv6Header*)(data + 14);
		CHAR strSaddr[INET6_ADDRSTRLEN] = { 0 };
		inet_ntop(AF_INET6, &iph->saddr, strSaddr, INET6_ADDRSTRLEN);

		CHAR strDaddr[INET6_ADDRSTRLEN] = { 0 };
		inet_ntop(AF_INET6, &iph->daddr, strDaddr, INET6_ADDRSTRLEN);

		std::vector<std::wstring> attributes;
		CapNetOutStreamW srcOwss;
		CapNetOutStreamW dstOwss;

		srcOwss << "源地址: " << strSaddr;
		dstOwss << "目的地址: " << strDaddr;

		attributes.push_back(srcOwss);
		attributes.push_back(dstOwss);

		PTreeNode last = GenAttributes(ipv6Node, attributes);
		return ipv6Node;
	}

	PTreeNode GenArpInfo(const UCHAR* data)
	{
		PTreeNode arpNode = new TreeNode{ 0 };
		arpNode->text = NewWStr(L"Arp");

		ArpHeader* arpHdr;
		arpHdr = (ArpHeader*)(data + 14);

		USHORT hardwareType = ntohs(arpHdr->hardwareType);
		USHORT protocolType = ntohs(arpHdr->protocolType);
		INT hardwareLength = arpHdr->hardwareLength;
		INT protocolLength = arpHdr->protocolLength;
		USHORT operationCode = ntohs(arpHdr->operationCode);

		std::vector<std::wstring> attributes;
		CapNetOutStreamW srcOwss;
		CapNetOutStreamW dstOwss;
		CapNetOutStreamW typeOwss;

		srcOwss << L"源MAC地址: ";
		for (INT x = 0; x < 6; x++)
			srcOwss << srcOwss.Format(L"%X:", arpHdr->sourceEthernetAddress[x]);

		dstOwss << L"目标MAC地址: ";
		for (INT x = 0; x < 6; x++)
			dstOwss << dstOwss.Format(L"%X:", arpHdr->destinationEthernetAddress[x]);

		typeOwss << L"类型: ";
		switch (operationCode)
		{
		case 1: typeOwss << L"ARP 请求"; break;
		case 2: typeOwss << L"ARP 应答 "; break;
		case 3: typeOwss << L"RARP 请求 "; break;
		case 4: typeOwss << L"RARP 应答 "; break;
		default: break;
		}

		attributes.push_back(srcOwss);
		attributes.push_back(dstOwss);
		attributes.push_back(typeOwss);

		PTreeNode last = GenAttributes(arpNode, attributes);
		return arpNode;
	}

	PTreeNode GenTcpInfo(const UCHAR* data)
	{
		PTreeNode tcpNode = new TreeNode{ 0 };
		tcpNode->text = NewWStr(L"Tcp");

		TcpHeader* tcpHdr;
		tcpHdr = (TcpHeader*)(data + 14 + 20);
		USHORT sport = ntohs(tcpHdr->sourPort);
		USHORT dport = ntohs(tcpHdr->destPort);
		USHORT window = ntohs(tcpHdr->windowSize);
		USHORT offset = (tcpHdr->offset & 0x0f) * 4;
		USHORT urgFlag = (tcpHdr->flags & 0x20) != 0;
		USHORT ackFlag = (tcpHdr->flags & 0x10) != 0;
		USHORT pshFlag = (tcpHdr->flags & 0x08) != 0;
		USHORT rstFlag = (tcpHdr->flags & 0x04) != 0;
		USHORT synFlag = (tcpHdr->flags & 0x02) != 0;
		USHORT finFlag = (tcpHdr->flags & 0x01) != 0;
		UINT seq = ntohl(tcpHdr->sequNum);
		UINT ack = ntohl(tcpHdr->cknowledgeNum);
		USHORT surgent = ntohs(tcpHdr->surgentPointer);

		std::vector<std::wstring> attributes;
		CapNetOutStreamW srcOwss;
		CapNetOutStreamW dstOwss;
		CapNetOutStreamW windowOwss;
		CapNetOutStreamW offsetOwss;
		CapNetOutStreamW flagOwss;
		CapNetOutStreamW seqOwss;
		CapNetOutStreamW ackOwss;
		CapNetOutStreamW surgentOwss;

		srcOwss << "源端口: " << sport;
		dstOwss << "目的端口: " << dport;
		windowOwss << "窗口大小: " << window;
		offsetOwss << "偏移: " << offset;
		flagOwss << flagOwss.Format(L"标志: URG=%d,ACK=%d,PSH=%d,RST=%d,SYN=%d,FIN=%d ", urgFlag, ackFlag, pshFlag, rstFlag, synFlag, finFlag);
		seqOwss << "序列号: " << seq;
		ackOwss << "确认号: " << ack;
		surgentOwss << "紧急数据偏移: " << surgent;

		attributes.push_back(srcOwss);
		attributes.push_back(dstOwss);
		attributes.push_back(windowOwss);
		attributes.push_back(offsetOwss);
		attributes.push_back(flagOwss);
		attributes.push_back(seqOwss);
		attributes.push_back(ackOwss);
		attributes.push_back(surgentOwss);

		PTreeNode last = GenAttributes(tcpNode, attributes);

		if (IsHttp(data))
		{
			last->firstBrother = GenHttpInfo(data);
		}
		else if (IsHttps(data))
		{
			last->firstBrother = GenHttpsInfo(data);
		}
		return tcpNode;
	}

	PTreeNode GenUdpInfo(const UCHAR* data)
	{
		return NULL;
	}

	PTreeNode GenIcmpInfo(const UCHAR* data)
	{
		return NULL;
	}

	PTreeNode GenHttpInfo(const UCHAR* data)
	{
		return NULL;
	}

	PTreeNode GenHttpsInfo(const UCHAR* data)
	{
		return NULL;
	}
}