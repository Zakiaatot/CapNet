#include <afxdlgs.h>
#include "CapNetLoadButton.h"
#include "CapNetUtils.h"
#include "CapNetGUIDlg.h"

BEGIN_MESSAGE_MAP(CapNetLoadButton, CButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

VOID CapNetLoadButton::Init()
{
	return VOID();
}

VOID CapNetLoadButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (pApp_->m_pacPool.GetItemCount() > 0)
	{
		INT num = CapNetUtils::AlertQueW(L"确认清空并从文件中加载？");
		if (num == 7)
		{
			return;
		}
	}
	pApp_->m_pacPool.DeleteAllItems();
	pApp_->m_pacPool.SetItemCount(0);
	CString strFilter = "CapNet Data File (*.pcap)|*.pcap";
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter.GetBuffer(), this);
	if (dlg.DoModal() == IDOK)
	{
		auto res = Singleton<CapNetCore>::Instance().LoadFile(pApp_->m_pacPool.ListenPackLoop, NULL, dlg.GetPathName());
		if (res)
		{
			CapNetUtils::AlertInfoA("加载成功！");
			pApp_->m_info.SuccessW(L"加载成功！");
		}
		else
		{
			CapNetUtils::AlertErrorA(res.msg.c_str());
			pApp_->m_info.ErrorW(L"加载失败！");
		}
	}
	else
	{
		return;
	}
}
