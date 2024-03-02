#include "CapNetListenButton.h"
#include "CapNetPackPoolListCtrl.h"
#include "CapNetGUIDlg.h"
#include "Utils.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CapNetListenButton, CMFCButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

VOID CapNetListenButton::Init()
{
	CMFCButton::SetMouseCursorHand();
}

VOID CapNetListenButton::ListeningStyle()
{
	SetWindowTextW(L"ȡ������");
}

VOID CapNetListenButton::UnlisteningStyle()
{
	SetWindowTextW(L"��ʼ����");
}

VOID CapNetListenButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	EnableWindow(0);
	CButton::OnLButtonUp(nFlags, point);
	if (isListening_)
	{

		Singleton<CapNetCore>::Instance().EndListen();
		UnlisteningStyle();
		isListening_ = FALSE;
	}
	else
	{
		int num = 0;
		if (pApp_->m_pacPool.GetItemCount() > 0)
		{
			num = Utils::AlertQueW(L"ȷ����ղ���ʼ�µļ�����");
			if (num == 7)
			{
				EnableWindow(1);
				return;
			}
		}
		pApp_->m_pacPool.DeleteAllItems();
		pApp_->m_pacPool.SetItemCount(0);
		CString ip;
		pApp_->m_ipEdit.GetWindowTextW(ip);
		if (ip.GetLength() > 0 && !CapNetIpEdit::CheckIpAddressW(ip.GetString()))
		{
			Utils::AlertErrorW(L"����ָ��IP��ַ��ʽ");
			EnableWindow(1);
			return;
		}

		auto res = Singleton<CapNetCore>::Instance().BeginListen
		(
			pApp_->m_devSelector.GetCurSel(),
			L"eth",
			L"",
			CapNetPackPoolListCtrl::ListenPackLoop,
			CapNetPackPoolListCtrl::ListenPackEnd
		);
		if (res)
		{
			isListening_ = TRUE;
			ListeningStyle();
		}
		else
		{
			Utils::AlertErrorA(res.msg.c_str());
		};
	}
	EnableWindow(1);
}
