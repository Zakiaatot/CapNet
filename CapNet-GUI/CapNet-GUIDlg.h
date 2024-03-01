#ifndef _CAPNET_GUI_DLG_H_
#define _CAPNET_GUI_DLG_H_

#include <afxdialogex.h>

class CCapNetGUIDlg : public CDialogEx
{
public:
	CCapNetGUIDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAPNETGUI_DIALOG };
#endif
	CComboBox m_devSelector;
	CEdit m_ipEdit;
	afx_msg void OnCbnSelchangeCombo1();
	void Exit() { CDialogEx::OnCancel(); };

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CListCtrl m_pacPool;
	CComboBox m_pacType;
private:
};
#endif // !_CAPNET_GUI_DLG_H_


