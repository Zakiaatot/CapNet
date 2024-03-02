#ifndef _CAPNET_PACTYPE_COMBOBOX_H_
#define _CAPNET_PACTYPE_COMBOBOX_H_

#include <afxcmn.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"

class CapNetPacTypeComboBox :public CComboBox, public CapNetCommon
{
public:
	explicit CapNetPacTypeComboBox(CCapNetGUIDlg* pApp) :CapNetCommon(pApp) {};
	VOID Init();
protected:
	DECLARE_MESSAGE_MAP()
private:

};
#endif // !_CAPNET_PACTYPE_COMBOBOX_H_