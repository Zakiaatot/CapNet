#define _WIN32_WINNT_MAXVER
#include "CapNetDevSelectorComboBox.h"
#include "CapNetGUIDlg.h"
#include "CapNetUtils.h"

BEGIN_MESSAGE_MAP(CapNetDevSelectorComboBox, CComboBox)

END_MESSAGE_MAP()

VOID CapNetDevSelectorComboBox::Init()
{
	CapNetCore::StatusVoid init = Singleton<CapNetCore>::Instance().Init();
	if (!init)
	{
		CapNetUtils::AlertErrorA(init.msg.c_str());
		pApp_->Exit();
	}
	auto ret = Singleton<CapNetCore>::Instance().FindAllDev();
	if (!ret)
	{
		CapNetUtils::AlertErrorA(ret.msg.c_str());
		pApp_->Exit();
	}
	else
	{
		for (auto i = ret.ret.begin(); i != ret.ret.end(); i++)
		{
			CA2W ca2w(i->second.c_str());
			std::wstring ws(ca2w);
			AddString(ws.c_str());
		}
		SetCurSel(0);
	}
}
