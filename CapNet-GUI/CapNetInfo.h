#ifndef _CAPNET_INFO_H_
#define _CAPNET_INFO_H_

#include <afxcmn.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"

class CapNetInfo :public CStatic, public CapNetCommon
{
public:
	explicit CapNetInfo(CCapNetGUIDlg* pApp) :CapNetCommon(pApp) {};
	VOID Init();
	VOID InfoW(const WCHAR* msg, ...);
	VOID ErrorW(const WCHAR* msg, ...);
	VOID SuccessW(const WCHAR* msg, ...);
	VOID WarningW(const WCHAR* msg, ...);
};


#endif // !_CAPNET_INFO_H_

