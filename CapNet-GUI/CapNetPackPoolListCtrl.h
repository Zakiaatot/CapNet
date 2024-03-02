#ifndef _CAPNET_PACKPOOL_LISTCTRL_H_
#define _CAPNET_PACKPOOL_LISTCTRL_H_

#include <afxcmn.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"


class CapNetPackPoolListCtrl :public CListCtrl, public CapNetCommon
{
public:
	explicit CapNetPackPoolListCtrl(CCapNetGUIDlg* pApp) :CapNetCommon(pApp) {};
	VOID Init();
	static VOID ListenPackLoop(CapNetCore::Pack& pack);
	static VOID ListenPackEnd(std::wstring& msg);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
private:

};
#endif // !_CAPNET_PACKPOOL_LISTCTRL_H_


