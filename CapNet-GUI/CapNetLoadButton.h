#ifndef _CAPNET_LOAD_BUTTON_H_
#define _CAPNET_LOAD_BUTTON_H_

#include <afxbutton.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"

class CCapNetGUIDlg;

class CapNetLoadButton :public CButton, public CapNetCommon< CCapNetGUIDlg>
{
public:
	explicit CapNetLoadButton(CCapNetGUIDlg* pApp)
		:CapNetCommon(pApp)
	{};
	VOID Init();
protected:
	DECLARE_MESSAGE_MAP()
private:
	afx_msg VOID OnLButtonUp(UINT nFlags, CPoint point);
};
#endif // !_CAPNET_LOAD_BUTTON_H_



