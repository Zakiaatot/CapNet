#ifndef _CAPNET_ABOUT_DLG_H_
#define _CAPNET_ABOUT_DLG_H_

#include <afxdialogex.h>

class CapNetAboutDlg : public CDialogEx
{
public:
	CapNetAboutDlg();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
protected:
	DECLARE_MESSAGE_MAP()
};

#endif // !_CAPNET_ABOUT_DLG_H_


