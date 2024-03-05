#ifndef _CAPNET_DETAIL_PARSE_H_
#define _CAPNET_DETAIL_PARSE_H_

#include <Windows.h>
#include "CapNetCore.h"

namespace CapNetDetailParse
{
	// Type use
	using TreeNode = typename CapNetCore::PacDetailTreeNode;
	using TreeRoot = typename CapNetCore::PacDetailTreeRoot;
	using PTreeNode = typename CapNetCore::PPacDetailTreeNode;
	using Tree = typename CapNetCore::PacDetailTree;

	// Export
	Tree CreataDetailTree(const UCHAR* data);
	VOID DestroyDetailTree(Tree& tree);

	// Gen
	PTreeNode GenEtherInfo(const UCHAR* data);
	PTreeNode GenIpv4Info(const UCHAR* data);
	PTreeNode GenIpv6Info(const UCHAR* data);
	PTreeNode GenArpInfo(const UCHAR* data);
	PTreeNode GenTcpInfo(const UCHAR* data);
	PTreeNode GenUdpInfo(const UCHAR* data);
	PTreeNode GenIcmpInfo(const UCHAR* data);
	PTreeNode GenHttpInfo(const UCHAR* data);
	PTreeNode GenHttpsInfo(const UCHAR* data);
};


#endif // !_CAPNET_DETAIL_PARSE_H_

