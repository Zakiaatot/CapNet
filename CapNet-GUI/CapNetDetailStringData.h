#ifndef _CAPNET_DETAIL_STRING_DATA_H_
#define _CAPNET_DETAIL_STRING_DATA_H_

#include <afxcmn.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"

class CapNetDetailDlg;


class CapNetDetailStringData :public CEdit, public CapNetCommon<CapNetDetailDlg>
{
public:
	explicit CapNetDetailStringData(CapNetDetailDlg* pApp) :CapNetCommon(pApp) {};
	VOID Init();
	VOID SetData(std::vector<BYTE>& byteVec);
};

#endif // !_CAPNET_DETAIL_STRING_DATA_H_

