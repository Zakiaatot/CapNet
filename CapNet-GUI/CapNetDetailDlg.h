#ifndef _CAPNET_DETAIL_DLG_H_
#define _CAPNET_DETAIL_DLG_H_

#include <afxdialogex.h>
#include "CapNetDetailRawDataPool.h"
#include "CapNetDetailStringData.h"

class CapNetDetailDlg : public CDialogEx
{
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	virtual BOOL OnInitDialog(); // Init Hook
protected:
	DECLARE_MESSAGE_MAP()
public:
	CapNetDetailDlg(UINT pacId);
	VOID Exit() { CDialogEx::OnCancel(); };
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif
	CapNetDetailRawDataPool m_rawDataPool;
	CapNetDetailStringData m_stringData;
	UINT m_packId;
};


#endif // _CAPNET_DETAIL_DLG_H_


