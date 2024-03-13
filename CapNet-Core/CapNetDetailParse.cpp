#include <vector>
#include <map>
#include <atlstr.h>
#include <regex>
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

static inline VOID DelWstr(WCHAR* str)
{
	delete[] str;
}

std::string FormatKey(std::string& str) {
	if (str[0] >= 'a' && str[0] <= 'z') {
		str[0] = str[0] + 'A' - 'a';
	}
	size_t position = 0;
	while ((position = str.find("-", position)) != std::string::npos) {
		if (str[position + 1] >= 'a' && str[position + 1] <= 'z') {
			str[position + 1] = str[position + 1] + 'A' - 'a';
		}
		position++;
	}
	return str;
}

static std::map<std::string, std::string>  ParseHttp(const UCHAR* data)
{
	std::string buf((char*)(data + 14 + 20 + 20));
	std::map<std::string, std::string> http;
	if (CapNetDetailParse::DetectSqlInjectByRegx(data))
	{
		http.insert(std::make_pair("����", "���ܴ���SQLע��"));
	}
	std::istringstream bufStream(buf);
	enum parts {
		startLine,
		headers,
		body
	};
	parts part = startLine;
	std::string line;
	std::string bodyString;
	while (getline(bufStream, line)) {
		switch (part)
		{
		case startLine:
		{
			std::istringstream lineStream(line);
			std::string tmp;
			lineStream >> tmp;
			if (tmp.find("HTTP") == std::string::npos) {
				http.insert(std::make_pair("Method", tmp));
				lineStream >> tmp;
				http.insert(std::make_pair("Path", tmp));
				lineStream >> tmp;
				http.insert(std::make_pair("Version", tmp));
			}
			else {
				http.insert(std::make_pair("Version", tmp));
				lineStream >> tmp;
				http.insert(std::make_pair("Status Code", tmp));
				lineStream >> tmp;
				http.insert(std::make_pair("Status Text", tmp));
			}
			part = headers;
			break;
		}
		case headers:
		{
			if (line.size() == 1) {
				part = body;
				break;
			}
			auto pos = line.find(":");
			if (pos == std::string::npos)
				continue;
			std::string tmp1(line, 0, pos);
			std::string tmp2(line, pos + 2);
			http.insert(std::make_pair(FormatKey(tmp1), tmp2));
			break;
		}
		case body:
		{
			bodyString.append(line);
			bodyString.push_back('\n');
			break;
		}
		default:
			break;
		}
	}
	http.insert(std::make_pair("Body", bodyString));
	return http;
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

		USHORT etherType = ntohs(ethHdr->etherType);  // ��̫������
		UCHAR* etherSrc = ethHdr->etherSHost;         // ��̫��ԭʼMAC��ַ
		UCHAR* etherDst = ethHdr->etherDHost;         // ��̫��Ŀ��MAC��ַ

		std::vector<std::wstring> attributes;
		CapNetOutStreamW srcOwss;
		CapNetOutStreamW dstOwss;
		CapNetOutStreamW typeOwss;

		srcOwss << srcOwss.Format(L"ԴMAC��ַ: %02X:%02X:%02X:%02X:%02X:%02X", etherSrc[0], etherSrc[1], etherSrc[2], etherSrc[3], etherSrc[4], etherSrc[5]);
		dstOwss << dstOwss.Format(L"Ŀ��MAC��ַ: %02X:%02X:%02X:%02X:%02X:%02X", etherDst[0], etherDst[1], etherDst[2], etherDst[3], etherDst[4], etherDst[5]);
		typeOwss << typeOwss.Format(L"����: %04X", etherType);

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

		UCHAR version = iph->version;
		UCHAR hdrLen = iph->headerLength * 4;
		UCHAR priority = (iph->tos) >> 5;
		UCHAR service = (iph->tos >> 1) & 0x0f;
		USHORT totalLen = ntohs(iph->totlaLength);
		USHORT iden = ntohs(iph->identification);
		USHORT df = (ntohs(iph->flagsOffset) >> 14) & 0x01;
		USHORT mf = (ntohs(iph->flagsOffset) >> 13) & 0x01;
		USHORT offset = ntohs(iph->flagsOffset) & 0x1fff;
		USHORT checkSum = ntohs(iph->checkSum);
		UCHAR ttl = iph->timeToLive;
		UCHAR proto = iph->protocol;

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

		verOwss << L"Э��汾: " << version;
		hdrLenOwss << L"�ײ�����: " << hdrLen;
		serviceOwss << L"��������: Priority=" << priority << L" Service=" << service;
		totalLenOwss << L"�ܳ���: " << totalLen;
		idenOwss << L"��ʶ: " << iden;
		flagOwss << L"��־λ: DF=" << df << L" MF=" << mf;
		offsetOwss << L"Ƭƫ��: " << offset;
		liveOwss << L"��������: " << ttl;
		protocolOwss << L"Э������: " << proto;
		checkSumOwss << L"�ײ�У���: " << checkSum;
		srcOwss << L"Դ��ַ: " << strSaddr;
		dstOwss << L"Ŀ�ĵ�ַ: " << strDaddr;

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

		srcOwss << L"Դ��ַ: " << strSaddr;
		dstOwss << L"Ŀ�ĵ�ַ: " << strDaddr;

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
		UCHAR hardwareLength = arpHdr->hardwareLength;
		UCHAR protocolLength = arpHdr->protocolLength;
		USHORT operationCode = ntohs(arpHdr->operationCode);

		std::vector<std::wstring> attributes;
		CapNetOutStreamW srcOwss;
		CapNetOutStreamW dstOwss;
		CapNetOutStreamW typeOwss;

		srcOwss << L"ԴMAC��ַ: ";
		for (INT x = 0; x < 6; x++)
		{
			if (x != 5)
				srcOwss << srcOwss.Format(L"%X:", arpHdr->sourceEthernetAddress[x]);
			else
				srcOwss << srcOwss.Format(L"%X", arpHdr->sourceEthernetAddress[x]);
		}

		dstOwss << L"Ŀ��MAC��ַ: ";
		for (INT x = 0; x < 6; x++)
		{
			if (x != 5)
				dstOwss << srcOwss.Format(L"%X:", arpHdr->destinationEthernetAddress[x]);
			else
				dstOwss << srcOwss.Format(L"%X", arpHdr->destinationEthernetAddress[x]);
		}

		typeOwss << L"����: ";
		switch (operationCode)
		{
		case 1: typeOwss << L"ARP ����"; break;
		case 2: typeOwss << L"ARP Ӧ�� "; break;
		case 3: typeOwss << L"RARP ���� "; break;
		case 4: typeOwss << L"RARP Ӧ�� "; break;
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
		UCHAR offset = tcpHdr->offset * 4;
		UCHAR urgFlag = (tcpHdr->flags & 0x20) != 0;
		UCHAR ackFlag = (tcpHdr->flags & 0x10) != 0;
		UCHAR pshFlag = (tcpHdr->flags & 0x08) != 0;
		UCHAR rstFlag = (tcpHdr->flags & 0x04) != 0;
		UCHAR synFlag = (tcpHdr->flags & 0x02) != 0;
		UCHAR finFlag = (tcpHdr->flags & 0x01) != 0;
		UINT seq = ntohl(tcpHdr->sequNum);
		UINT ack = ntohl(tcpHdr->cknowledgeNum);
		USHORT surgent = ntohs(tcpHdr->surgentPointer);
		USHORT checkSum = ntohs(tcpHdr->checkSum);

		std::vector<std::wstring> attributes;
		CapNetOutStreamW srcOwss;
		CapNetOutStreamW dstOwss;
		CapNetOutStreamW windowOwss;
		CapNetOutStreamW offsetOwss;
		CapNetOutStreamW flagOwss;
		CapNetOutStreamW seqOwss;
		CapNetOutStreamW ackOwss;
		CapNetOutStreamW surgentOwss;
		CapNetOutStreamW checkSumOwss;

		srcOwss << L"Դ�˿�: " << sport;
		dstOwss << L"Ŀ�Ķ˿�: " << dport;
		windowOwss << L"���ڴ�С: " << window;
		offsetOwss << L"ͷ����С: " << offset;
		flagOwss << flagOwss.Format(L"��־: URG=%d,ACK=%d,PSH=%d,RST=%d,SYN=%d,FIN=%d ", urgFlag, ackFlag, pshFlag, rstFlag, synFlag, finFlag);
		seqOwss << L"���к�: " << seq;
		ackOwss << L"ȷ�Ϻ�: " << ack;
		surgentOwss << L"��������ƫ��: " << surgent;
		checkSumOwss << L"У���: " << checkSum;

		attributes.push_back(srcOwss);
		attributes.push_back(dstOwss);
		attributes.push_back(windowOwss);
		attributes.push_back(offsetOwss);
		attributes.push_back(flagOwss);
		attributes.push_back(seqOwss);
		attributes.push_back(ackOwss);
		attributes.push_back(surgentOwss);
		attributes.push_back(checkSumOwss);

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
		PTreeNode udpNode = new TreeNode{ 0 };
		udpNode->text = NewWStr(L"Udp");

		UdpHeader* udpHdr;
		udpHdr = (UdpHeader*)(data + 14 + 20);

		USHORT sport = ntohs(udpHdr->sport);
		USHORT dport = ntohs(udpHdr->dport);
		USHORT dataLen = ntohs(udpHdr->dataLen);
		UCHAR proto = udpHdr->proto;

		std::vector<std::wstring> attributes;
		CapNetOutStreamW srcOwss;
		CapNetOutStreamW dstOwss;
		CapNetOutStreamW lenOwss;
		CapNetOutStreamW protocolOwss;

		srcOwss << L"Դ�˿�: " << sport;
		dstOwss << L"Ŀ�Ķ˿�: " << dport;
		lenOwss << L"���ݴ�С: " << dataLen;
		protocolOwss << L"Э������: " << proto;

		attributes.push_back(srcOwss);
		attributes.push_back(dstOwss);
		attributes.push_back(lenOwss);
		attributes.push_back(protocolOwss);

		PTreeNode last = GenAttributes(udpNode, attributes);

		return udpNode;
	}

	PTreeNode GenIcmpInfo(const UCHAR* data)
	{
		PTreeNode icmpNode = new TreeNode{ 0 };
		icmpNode->text = NewWStr(L"Icmp");

		IcmpHeader* icmpHdr;
		icmpHdr = (struct IcmpHeader*)(data + 14 + 20);

		UCHAR type = icmpHdr->type;
		UINT initTime = ntohl(icmpHdr->initTime);
		USHORT sendTime = ntohs(icmpHdr->sendTime);
		USHORT recvTime = ntohs(icmpHdr->recvTime);

		std::vector<std::wstring> attributes;
		CapNetOutStreamW initOwss;
		CapNetOutStreamW sendOwss;
		CapNetOutStreamW recvOwss;
		CapNetOutStreamW typeOwss;

		initOwss << L"����ʱ���: " << initTime;
		sendOwss << L"����ʱ���: " << sendTime;
		recvOwss << L"����ʱ���: " << recvTime;
		switch (type)
		{
		case 0: typeOwss << L"����: ����Ӧ����"; break;
		case 8: typeOwss << L"����: ����������"; break;
		default:break;
		}

		attributes.push_back(initOwss);
		attributes.push_back(sendOwss);
		attributes.push_back(recvOwss);
		attributes.push_back(typeOwss);

		PTreeNode last = GenAttributes(icmpNode, attributes);
		return icmpNode;
	}

	PTreeNode GenHttpInfo(const UCHAR* data)
	{
		PTreeNode httpNode = new TreeNode{ 0 };
		httpNode->text = NewWStr(L"Http");
		auto res = ParseHttp(data);
		std::vector<std::wstring> attributes;
		for (auto i = res.crbegin(); i != res.crend(); i++)
		{
			auto& f = (*i).first;
			auto& s = (*i).second;
			CA2W k(f.c_str());
			CA2W v(s.c_str());
			CapNetOutStreamW owss;
			owss << std::wstring(k) << ": " << std::wstring(v);
			attributes.push_back(owss);
		}
		PTreeNode last = GenAttributes(httpNode, attributes);
		return httpNode;
	}

	PTreeNode GenHttpsInfo(const UCHAR* data)
	{
		return NULL;
	}

	BOOL DetectSqlInjectByRegx(const UCHAR* data)
	{
		std::string buf((char*)(data + 14 + 20 + 20));
		std::regex sqlInjectionPatterns[] = {
			std::regex("union\\s+select", std::regex::icase), // UNION SELECT ���
			std::regex("\\b(and|or)\\b\\s+\\b(\\d+)\\b", std::regex::icase), // AND/OR ���������
			std::regex("'\\s*(or|and)\\s*'", std::regex::icase), // �����Ű����� OR/AND
			std::regex("\\b(sleep|benchmark)\\s*\\(", std::regex::icase), // ˯��/��׼���Ժ���
			std::regex("(\\%27)|(\\')\\s*\\+\\s*\\+", std::regex::icase), // �ַ�������
			std::regex("(\\d+)\\s*(\\=|\\>|\\<|\\!)", std::regex::icase) // ������Ƚ���������
		};

		for (const auto& pattern : sqlInjectionPatterns) {
			if (std::regex_search(buf, pattern)) {
				return TRUE;
			}
		}

		return FALSE;
	}
}