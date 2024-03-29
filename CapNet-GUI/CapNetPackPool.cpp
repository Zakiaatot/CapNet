#include "CapNetPackPool.h"
#include "CapNetUtils.h"
#include "CapNetDetailDlg.h"
#include "resource.h"

static CapNetPackPoolListCtrl* gPackPoolListCtrl = NULL;
static UINT gRClickIndex = 0;

BEGIN_MESSAGE_MAP(CapNetPackPoolListCtrl, CListCtrl)
	ON_WM_NCCALCSIZE()
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnNMRClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDBLClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_COMMAND(ID_32773, &CapNetPackPoolListCtrl::OnClickPacDetail)
END_MESSAGE_MAP()


VOID CapNetPackPoolListCtrl::Init()
{
	// Pac Pool
	CRect rect;
	GetClientRect(&rect);

	InsertColumn(0, _T("No."), LVCFMT_LEFT, 60, 0);
	InsertColumn(1, _T("Time"), LVCFMT_LEFT, 200, 1);
	InsertColumn(2, _T("Source"), LVCFMT_LEFT, 200, 2);
	InsertColumn(3, _T("Destination"), LVCFMT_LEFT, 200, 3);
	InsertColumn(4, _T("Protocol"), LVCFMT_LEFT, 110, 4);
	InsertColumn(5, _T("Length"), LVCFMT_LEFT, 100, 5);
	InsertColumn(6, _T("Info"), LVCFMT_LEFT, rect.Width() - 60 - 200 - 200 - 200 - 110 - 100, 6);

	gPackPoolListCtrl = this;

	//获得原有风格
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= ~(LVS_TYPEMASK);
	dwStyle &= ~(LVS_EDITLABELS);
	//设置新风格
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | LVS_REPORT | LVS_NOLABELWRAP | LVS_SHOWSELALWAYS);
	//设置扩展风格
	DWORD styles = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyleEx(m_hWnd, styles, styles);
}

VOID CapNetPackPoolListCtrl::ListenPackLoop(CapNetCore::Pack& pack)
{
	if (!gPackPoolListCtrl)
		return;
	UINT count = pack.no;
	gPackPoolListCtrl->InsertItem(count, L"");
	if (pack.isSqlInject)
		gPackPoolListCtrl->SetColor(count, 0, RGB(255, 0, 0));
	WCHAR buf[16] = { 0 };
	wsprintfW(buf, L"%u", count);
	gPackPoolListCtrl->SetItemText(count, 0, buf);
	gPackPoolListCtrl->SetItemText(count, 1, pack.time.c_str());
	gPackPoolListCtrl->SetItemText(count, 2, pack.source.c_str());
	gPackPoolListCtrl->SetItemText(count, 3, pack.dest.c_str());
	gPackPoolListCtrl->SetItemText(count, 4, pack.protocol.c_str());
	wsprintfW(buf, L"%u", pack.length);
	gPackPoolListCtrl->SetItemText(count, 5, buf);
	gPackPoolListCtrl->SetItemText(count, 6, pack.info.c_str());
	gPackPoolListCtrl->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

VOID CapNetPackPoolListCtrl::ListenPackEnd(std::wstring& msg)
{
	if (!gPackPoolListCtrl)
		return;
	CapNetUtils::AlertErrorW(msg.c_str());
}

VOID CapNetPackPoolListCtrl::SetColor(int iRow, int iCol, COLORREF color)
{
	ItemColor itemColor;
	itemColor.m_iRow = iRow;
	itemColor.m_iCol = iCol;
	itemColor.m_TextColor = color;
	m_itemColors.push_back(itemColor);
}

VOID CapNetPackPoolListCtrl::ClearColor()
{
	m_itemColors.clear();
}


VOID CapNetPackPoolListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	ModifyStyle(WS_HSCROLL, 0, 0);
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}


void CapNetPackPoolListCtrl::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int t = pNMItemActivate->iItem;
	if (t < 0)
	{
		LVHITTESTINFO tHitTest{};
		tHitTest.pt = pNMItemActivate->ptAction;
		t = (int)SendMessage(LVM_SUBITEMHITTEST, 0, reinterpret_cast<LPARAM>(&tHitTest));
	}
	if (t == -1)
		return;
	int nItem = GetNextItem(-1, LVNI_SELECTED); //表示获取上一次被设置点中的某项；
	if (nItem >= 0 && nItem != t)
		SetItemState(nItem, 0, -1);//0表示不被选中，-1表示不高亮；
	SetItemState(t, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	*pResult = 0;
}


void CapNetPackPoolListCtrl::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int t = pNMItemActivate->iItem;
	if (t < 0)
	{
		LVHITTESTINFO tHitTest{};
		tHitTest.pt = pNMItemActivate->ptAction;
		t = (int)SendMessage(LVM_SUBITEMHITTEST, 0, reinterpret_cast<LPARAM>(&tHitTest));
	}
	if (t == -1)
		return;
	if (pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));
		CMenu menu;
		//添加线程操作
		VERIFY(menu.LoadMenu(IDR_MENU1));			//这里是我们在1中定义的MENU的文件名称
		gRClickIndex = t;
		CMenu* popup = menu.GetSubMenu(0);
		ASSERT(popup != NULL);
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	*pResult = 0;
}

VOID CapNetPackPoolListCtrl::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	//当pLVCD结构体中nmcd成员的dwDrawStage状态为CDDS_ITEMPREPAINT | CDDS_SUBITEM时，我们就可以根据行和列去设置
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
	{
		//先将字体颜色和背景颜色设为系统默认                                                               
		pLVCD->clrTextBk = 16777215;
		pLVCD->clrText = 0;
		//遍历字体颜色表
		for (int i = 0; i < m_itemColors.size(); i++)
		{
			//判断该行中是否存在需要改变颜色的单元格
			if (m_itemColors[i].m_iRow == pLVCD->nmcd.dwItemSpec)
			{
				//设置单元格背景颜色，我这里是默认将一整行都设为分红色
				//如果需要设为其他颜色，或者单独设置某一单元格的颜色，可以效仿字体颜色的方式去添加
				pLVCD->clrTextBk = RGB(255, 228, 225);
				//判断该单元格字体是否需要改变颜色
				if (m_itemColors[i].m_iCol == pLVCD->iSubItem)
				{
					pLVCD->clrText = m_itemColors[i].m_TextColor;
					break;
				}
			}
		}
		*pResult = CDRF_DODEFAULT;
	}
}

VOID CapNetPackPoolListCtrl::OnNMDBLClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int t = pNMItemActivate->iItem;
	if (t < 0)
	{
		LVHITTESTINFO tHitTest{};
		tHitTest.pt = pNMItemActivate->ptAction;
		t = (int)SendMessage(LVM_SUBITEMHITTEST, 0, reinterpret_cast<LPARAM>(&tHitTest));
	}
	if (t == -1)
		return;
	CapNetDetailDlg detailWin(t);
	detailWin.DoModal();
}


VOID CapNetPackPoolListCtrl::OnClickPacDetail()
{
	CapNetDetailDlg detailWin(gRClickIndex);
	detailWin.DoModal();
}
