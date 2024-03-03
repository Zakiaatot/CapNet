#include "CapNetListenButton.h"
#include "CapNetPackPoolListCtrl.h"
#include "CapNetGUIDlg.h"
#include "CapNetUtils.h"

BEGIN_MESSAGE_MAP(CapNetListenButton, CButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

VOID CapNetListenButton::Init()
{
	//CMFCButton::SetMouseCursorHand();
}

VOID CapNetListenButton::ListeningStyle()
{
	pApp_->m_devSelector.EnableWindow(0);
	isListening_ = TRUE;
	SetWindowTextW(L"取消监听");
	pApp_->m_info.SuccessW(L"开始监听");
	pApp_->m_pacPool.SetFocus();
}

VOID CapNetListenButton::UnlisteningStyle()
{
	pApp_->m_devSelector.EnableWindow(1);
	isListening_ = FALSE;
	SetWindowTextW(L"开始监听");
	pApp_->m_info.SuccessW(L"取消监听");
}

VOID CapNetListenButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	EnableWindow(0);
	CButton::OnLButtonUp(nFlags, point);
	if (isListening_)
	{

		Singleton<CapNetCore>::Instance().EndListen();
		UnlisteningStyle();
	}
	else
	{
		int num = 0;
		if (pApp_->m_pacPool.GetItemCount() > 0)
		{
			num = CapNetUtils::AlertQueW(L"确认清空并开始新的监听？");
			if (num == 7)
			{
				EnableWindow(1);
				return;
			}
		}
		pApp_->m_pacPool.DeleteAllItems();
		pApp_->m_pacPool.SetItemCount(0);
		CString rule;
		pApp_->m_ruleEdit.GetWindowTextW(rule);

		auto res = Singleton<CapNetCore>::Instance().BeginListen
		(
			pApp_->m_devSelector.GetCurSel(),
			rule.GetString(),
			CapNetPackPoolListCtrl::ListenPackLoop,
			CapNetPackPoolListCtrl::ListenPackEnd
		);
		if (res)
		{
			ListeningStyle();
		}
		else
		{
			CapNetUtils::AlertErrorA(res.msg.c_str());
		};
	}
	EnableWindow(1);
}
