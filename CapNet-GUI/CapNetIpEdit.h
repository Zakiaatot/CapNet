#ifndef _CAPNET_IP_EDIT_H_
#define _CAPNET_IP_EDIT_H_

#include <afxcmn.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"


class CapNetIpEdit :public CComboBox, public CapNetCommon
{
public:
	explicit CapNetIpEdit(CCapNetGUIDlg* pApp) :CapNetCommon(pApp) {};
	VOID Init();
	static BOOL CheckIpAddressW(const WCHAR* ip);
	static BOOL CheckIpAddressA(const CHAR* ip);
protected:
	DECLARE_MESSAGE_MAP()
private:
};

#endif // !_CAPNET_IP_EDIT_H_

