#include "CapNetPackPoolListCtrl.h"
#include "CapNetUtils.h"

static CapNetPackPoolListCtrl* gPackPoolListCtrl = NULL;

BEGIN_MESSAGE_MAP(CapNetPackPoolListCtrl, CListCtrl)
	ON_WM_NCCALCSIZE()
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
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
}

VOID CapNetPackPoolListCtrl::ListenPackLoop(CapNetCore::Pack& pack)
{
	if (!gPackPoolListCtrl)
		return;
	UINT count = pack.no;
	gPackPoolListCtrl->InsertItem(count, L"");
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


VOID CapNetPackPoolListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	ModifyStyle(WS_HSCROLL, 0, 0);
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}


void CapNetPackPoolListCtrl::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	SetItemState(pNMItemActivate->iItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	*pResult = 0;
}
