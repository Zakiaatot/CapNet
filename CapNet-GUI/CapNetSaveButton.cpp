#define _CRT_SECURE_NO_WARNINGS
#include <afxdlgs.h>
#include "CapNetSaveButton.h"
#include "CapNetGUIDlg.h"
#include "CapNet-Core/CapNetCore.h"
#include "CapNetUtils.h"
#include "CapNetStream.h"

BEGIN_MESSAGE_MAP(CapNetSaveButton, CButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

VOID CapNetSaveButton::Init()
{
	return VOID();
}

VOID CapNetSaveButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	auto res = Singleton<CapNetCore>::Instance().IsSaveable();
	if (!res)
	{
		CapNetUtils::AlertWarningA(res.msg.c_str());
		pApp_->m_info.ErrorW(L"保存失败!");
		return;
	}
	CString filePath;
	CString	strFilter;
	time_t t;
	time(&t);
	struct tm* timeinfo;
	timeinfo = localtime(&t);
	char buffer[80];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", timeinfo);
	CapNetOutStreamA aoss;
	aoss << buffer;
	strFilter = "CapNet Data File (*.pcap)";
	CA2W ca2w(aoss.str().c_str());
	CFileDialog dlg(FALSE, _T("pcap"), ca2w, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter.GetBuffer(), this);
	if (dlg.DoModal() == IDOK)
	{
		filePath = dlg.GetPathName();
		res = Singleton<CapNetCore>::Instance().SaveFile(filePath.GetString());
		if (res)
		{
			CapNetUtils::AlertInfoW(L"保存成功!");
			pApp_->m_info.SuccessW(L"保存成功!");
		}
		else
		{
			CapNetUtils::AlertErrorA(res.msg.c_str());
		}
	}
	else
	{
		return;
	}
}
