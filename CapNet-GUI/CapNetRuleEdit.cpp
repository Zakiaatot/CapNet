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
		pApp_->m_info.InfoW(L"�Ѹ��Ĺ��˹���Ϊ %s", rule.GetString());
		return;
	}
	auto res = Singleton<CapNetCore>::Instance().SetFilter(rule.GetString());
	if (res)
	{
		if (rule.GetLength() == 0)
		{
			pApp_->m_info.SuccessW(L"��ȡ�����˹���");
			return;
		}
		pApp_->m_info.SuccessW(L"�Ѹ��Ĺ��˹���Ϊ %s", rule.GetString());
	}
	else
	{
		CA2W ca2w(res.msg.c_str());
		pApp_->m_info.ErrorW(L"������˹��� %s %s  ", rule.GetString(), ca2w.m_szBuffer);
	}
}
