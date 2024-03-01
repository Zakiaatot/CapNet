#ifndef _CAPNET_GUI_H_
#define _CAPNET_GUI_H_

#include <afxwin.h>
#include "resource.h"

class CCapNetGUIApp : public CWinApp
{
public:
	CCapNetGUIApp();
public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};

extern CCapNetGUIApp theApp;

#endif // !_CAPNET_GUI_H_