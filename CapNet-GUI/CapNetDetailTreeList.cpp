#include "CapNetDetailTreeList.h"
#include "CapNetDetailDlg.h"
#include "CapNetUtils.h"
#include "CapNetStream.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CapNetDetailTreeList, CTreeCtrl)
END_MESSAGE_MAP()

CapNetDetailTreeList::~CapNetDetailTreeList()
{
	if (tree_)
		Singleton<CapNetCore>::Instance().DelDetailTree(tree_);
	CTreeCtrl::~CTreeCtrl();
}

VOID CapNetDetailTreeList::Init()
{
	auto root = Singleton<CapNetCore>::Instance().GenDetailTree(pApp_->m_packId);
	if (!root)
	{
		CapNetUtils::AlertErrorA(root.msg.c_str());
		pApp_->Exit();
	}
	if (root.ret == NULL)
	{
		CapNetUtils::AlertErrorW(L"未知错误！");
	}
	tree_ = root.ret;

	CapNetOutStreamW woss;
	woss << woss.Format(L"No. %d 报文解析", pApp_->m_packId);
	HTREEITEM begin = InsertItem(woss.str().c_str());
	InsertTree(tree_, begin);
	Expand(begin, TVE_EXPAND);
}

VOID CapNetDetailTreeList::InsertTree(CapNetCore::PacDetailTree tree, HTREEITEM father)
{
	if (!tree)
		return;
	auto h = InsertItem(tree->text, father);
	if (tree->firstBrother)
	{
		InsertTree(tree->firstBrother, father);
	}
	if (tree->firstChild)
	{
		InsertTree(tree->firstChild, h);
	}
	Expand(h, TVE_EXPAND);
}

