#include "CapNetRuleEdit.h"
#include "CapNetGUIDlg.h"
#include "CapNet-Core/CapNetCore.h"

BEGIN_MESSAGE_MAP(CapNetRuleEdit, CEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, OnEditChange)
END_MESSAGE_MAP()

VOID CapNetRuleEdit::Init()
{
	return VOID();
}

VOID CapNetRuleEdit::OnEditChange()
{

	CString rule;
	GetWindowTextW(rule);
	if (!pApp_->m_listenButton.GetListenStatus())
	{
		pApp_->m_info.InfoW(L"已更改过滤规则为 %s", rule.GetString());
		return;
	}
	auto res = Singleton<CapNetCore>::Instance().SetFilter(rule.GetString());
	if (res)
	{
		if (rule.GetLength() == 0)
		{
			pApp_->m_info.SuccessW(L"已取消过滤规则");
			return;
		}
		pApp_->m_info.SuccessW(L"已更改过滤规则为 %s", rule.GetString());
	}
	else
	{
		CA2W ca2w(res.msg.c_str());
		pApp_->m_info.ErrorW(L"请检查过滤规则 %s %s  ", rule.GetString(), ca2w.m_szBuffer);
	}
}
