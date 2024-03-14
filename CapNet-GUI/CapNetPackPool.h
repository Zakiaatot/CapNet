#ifndef _CAPNET_PACKPOOL_H_
#define _CAPNET_PACKPOOL_H_

#include <afxcmn.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"

class CCapNetGUIDlg;

struct ItemColor
{
	int m_iRow;
	int m_iCol;
	COLORREF m_TextColor;
};

class CapNetPackPoolListCtrl :public CListCtrl, public CapNetCommon<CCapNetGUIDlg>
{
public:
	explicit CapNetPackPoolListCtrl(CCapNetGUIDlg* pApp) :CapNetCommon(pApp), m_itemColors() {};
	VOID Init();
	static VOID ListenPackLoop(CapNetCore::Pack& pack);
	static VOID ListenPackEnd(std::wstring& msg);

	VOID SetColor(int iRow, int iCol, COLORREF color);
	VOID ClearColor();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg VOID OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg VOID OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg VOID OnNMDBLClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg VOID OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg VOID OnClickPacDetail();

	//用于保存某一行某一列的字体颜色
	std::vector<ItemColor> m_itemColors;
};
#endif // !_CAPNET_PACKPOOL_H_


