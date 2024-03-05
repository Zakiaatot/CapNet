#ifndef _CAPNET_DETAIL_TREE_LIST_H_
#define _CAPNET_DETAIL_TREE_LIST_H_

#include <afxcmn.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"

class CapNetDetailDlg;

class CapNetDetailTreeList :public CTreeCtrl, public CapNetCommon<CapNetDetailDlg>
{
public:
	explicit CapNetDetailTreeList(CapNetDetailDlg* pApp) :CapNetCommon(pApp) {};
	VOID Init();
protected:
	DECLARE_MESSAGE_MAP()
private:
};


#endif // !_CAPNET_DETAIL_TREE_LIST_H_

