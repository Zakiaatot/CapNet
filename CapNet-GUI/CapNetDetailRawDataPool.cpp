#include "CapNetDetailRawDataPool.h"
#include "CapNetStream.h"
#include "CapNetDetailDlg.h"
#include "CapNetUtils.h"

BEGIN_MESSAGE_MAP(CapNetDetailRawDataPool, CListCtrl)
	ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()

VOID CapNetDetailRawDataPool::Init()
{
	// Raw Data Pool
	CRect rect;
	GetClientRect(&rect);

	int byteWidth = (rect.Width() - 20) / 16;

	for (int i = 0; i < 16; i++)
	{
		CapNetOutStreamA aoss;
		aoss << "B" << std::setw(2) << std::setfill('0') << i;
		CA2W ca2w(aoss.str().c_str());
		InsertColumn(i + 1, ca2w, LVCFMT_CENTER, byteWidth, i + 1);
	}

	//获得原有风格
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= ~(LVS_TYPEMASK);
	dwStyle &= ~(LVS_EDITLABELS);
	//设置新风格
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | LVS_REPORT | LVS_NOLABELWRAP | LVS_SHOWSELALWAYS);
	//设置扩展风格
	DWORD styles = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyleEx(m_hWnd, styles, styles);

	// Get Raw Data
	auto res = Singleton<CapNetCore>::Instance().GetRawData(pApp_->m_packId);
	if (res)
	{
		int row = 0;
		int col = 0;
		InsertItem(0, L"");
		for (auto i = res.ret.begin(); i != res.ret.end(); i++)
		{
			CapNetOutStreamW woss;
			woss << woss.Format(L"%02X", *i);
			SetItemText(row, col, woss.str().c_str());
			col++;
			if (col == 16)
			{
				col = 0;
				row++;
				InsertItem(row, L"");
			}
		}
	}
	else
	{
		CapNetUtils::AlertErrorW(L"获取原始报文出错！");
		pApp_->Exit();
	}
}

VOID CapNetDetailRawDataPool::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	//ModifyStyle(WS_HSCROLL, 0, 0);
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}
