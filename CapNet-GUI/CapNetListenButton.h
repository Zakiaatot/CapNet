#ifndef _CAPNET_LISTEN_BUTTON_H_
#define _CAPNET_LISTEN_BUTTON_H_

#include <afxbutton.h>
#include "CapNetCommon.h"
#include "CapNet-Core/CapNetCore.h"

class CCapNetGUIDlg;

class CapNetListenButton :public CButton, public CapNetCommon< CCapNetGUIDlg>
{
public:
	explicit CapNetListenButton(CCapNetGUIDlg* pApp)
		:CapNetCommon(pApp),
		isListening_(FALSE)
	{};
	VOID Init();
	BOOL GetListenStatus() const { return isListening_; };
protected:
	DECLARE_MESSAGE_MAP()
private:
	BOOL isListening_;
	VOID ListeningStyle();
	VOID UnlisteningStyle();
	afx_msg VOID OnLButtonUp(UINT nFlags, CPoint point);
};
#endif // !_CAPNET_LISTEN_BUTTON_H_



