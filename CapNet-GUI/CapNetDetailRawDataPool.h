#ifndef _CAPNET_DETAIL_RAWDATA_POOL_H_
#define _CAPNET_DETAIL_RAWDATA_POOL_H_

#include <afxcmn.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"

class CapNetDetailDlg;

class CapNetDetailRawDataPool :public CListCtrl, public CapNetCommon<CapNetDetailDlg>
{
public:
	explicit CapNetDetailRawDataPool(CapNetDetailDlg* pApp) :CapNetCommon(pApp) {};
	VOID Init();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg VOID OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
};

#endif // !_CAPNET_DETAIL_RAWDATA_POOL_H_

