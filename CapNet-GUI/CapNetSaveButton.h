#ifndef _CAPNET_SAVE_BUTTON_H_
#define _CAPNET_SAVE_BUTTON_H_

#include <afxbutton.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"

class CCapNetGUIDlg;

class CapNetSaveButton :public CButton, public CapNetCommon< CCapNetGUIDlg>
{
public:
	explicit CapNetSaveButton(CCapNetGUIDlg* pApp)
		:CapNetCommon(pApp)
	{};
	VOID Init();
protected:
	DECLARE_MESSAGE_MAP()
private:
	afx_msg VOID OnLButtonUp(UINT nFlags, CPoint point);
};
#endif // !_CAPNET_SAVE_BUTTON_H_



