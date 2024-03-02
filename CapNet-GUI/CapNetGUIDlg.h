﻿#ifndef _CAPNET_GUI_DLG_H_
#define _CAPNET_GUI_DLG_H_

#include <afxdialogex.h>
#include "CapNetPackPoolListCtrl.h"
#include "CapNetPacTypeComboBox.h"
#include "CapNetDevSelectorComboBox.h"
#include "CapNetIpEdit.h"
#include "CapNetListenButton.h"

class CCapNetGUIDlg : public CDialogEx
{
public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAPNETGUI_DIALOG };
#endif
protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CCapNetGUIDlg(CWnd* pParent = nullptr);
	void Exit() { CDialogEx::OnCancel(); };

	CapNetPackPoolListCtrl m_pacPool;
	CapNetPacTypeComboBox m_pacType;
	CapNetListenButton m_listenButton;
	CapNetDevSelectorComboBox m_devSelector;
	CapNetIpEdit m_ipEdit;
};
#endif // !_CAPNET_GUI_DLG_H_

