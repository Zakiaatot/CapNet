#ifndef _CAPNET_DEVSELECTOR_COMBOBOX_H_
#define _CAPNET_DEVSELECTOR_COMBOBOX_H_


#include <afxcmn.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"

class CapNetDevSelectorComboBox :public CComboBox, public CapNetCommon
{
public:
	explicit CapNetDevSelectorComboBox(CCapNetGUIDlg* pApp) :CapNetCommon(pApp) {};
	VOID Init();
protected:
	DECLARE_MESSAGE_MAP()
private:
};

#endif // !_CAPNET_DEVSELECTOR_COMBOBOX_H_


