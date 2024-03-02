#include "CapNetListenButton.h"
#include "CapNetPackPoolListCtrl.h"
#include "CapNetGUIDlg.h"
#include "CapNetUtils.h"
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
	pApp_->m_devSelector.EnableWindow(0);
	pApp_->m_pacType.EnableWindow(0);
	pApp_->m_ipEdit.EnableWindow(0);
	isListening_ = TRUE;
	SetWindowTextW(L"ȡ������");
}

VOID CapNetListenButton::UnlisteningStyle()
{
	pApp_->m_devSelector.EnableWindow(1);
	pApp_->m_pacType.EnableWindow(1);
	pApp_->m_ipEdit.EnableWindow(1);
	isListening_ = FALSE;
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
	}
	else
	{
		int num = 0;
		if (pApp_->m_pacPool.GetItemCount() > 0)
		{
			num = CapNetUtils::AlertQueW(L"ȷ����ղ���ʼ�µļ�����");
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
			CapNetUtils::AlertErrorW(L"����ָ��IP��ַ��ʽ");
			EnableWindow(1);
			return;
		}
		CString type;
		int i = pApp_->m_pacType.GetCurSel();
		pApp_->m_pacType.GetLBText(i, type);

		auto res = Singleton<CapNetCore>::Instance().BeginListen
		(
			pApp_->m_devSelector.GetCurSel(),
			type.GetString(),
			ip.GetString(),
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
