#ifndef _CAPNET_DEVSELECTOR_H_
#define _CAPNET_DEVSELECTOR_H_


#include <afxcmn.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"

class CCapNetGUIDlg;

class CapNetDevSelectorComboBox :public CComboBox, public CapNetCommon<CCapNetGUIDlg>
{
public:
	explicit CapNetDevSelectorComboBox(CCapNetGUIDlg* pApp) :CapNetCommon(pApp) {};
	VOID Init();
protected:
	DECLARE_MESSAGE_MAP()
private:
	afx_msg VOID  OnSelectedChange();
};

#endif // !_CAPNET_DEVSELECTOR_H_


