#ifndef _CAPNET_RULE_EDIT_H_
#define _CAPNET_RULE_EDIT_H_

#include <afxcmn.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"


class CapNetRuleEdit :public CEdit, public CapNetCommon
{
public:
	explicit CapNetRuleEdit(CCapNetGUIDlg* pApp) :CapNetCommon(pApp) {};
	VOID Init();
protected:
	DECLARE_MESSAGE_MAP()
private:
	afx_msg VOID OnEditChange();
};

#endif // !_CAPNET_RULE_EDIT_H_

