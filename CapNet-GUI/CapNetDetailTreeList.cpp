#include "CapNetDetailTreeList.h"
#include "CapNetDetailDlg.h"
#include "CapNetUtils.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CapNetDetailTreeList, CTreeCtrl)
END_MESSAGE_MAP()

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
		CapNetUtils::AlertErrorW(L"Î´Öª´íÎó£¡");
	}

	HTREEITEM Grades = InsertItem(_T("Grade"), 0, 0);
	HTREEITEM Grade1 = InsertItem(_T("Grade 1"), 1, 1, Grades);
	HTREEITEM Grade2 = InsertItem(_T("Grade 2"), 1, 1, Grades);
	HTREEITEM Grade3 = InsertItem(_T("Grade 3"), 1, 1, Grades);
	Expand(Grades, TVE_EXPAND);

}

