#include "CapNetPacTypeComboBox.h"

BEGIN_MESSAGE_MAP(CapNetPacTypeComboBox, CComboBox)

END_MESSAGE_MAP()

VOID CapNetPacTypeComboBox::Init()
{
	AddString(L"eth");
	AddString(L"ip");
	AddString(L"arp");
	AddString(L"tcp");
	AddString(L"udp");
	AddString(L"icmp");
	AddString(L"http");
	AddString(L"https");
	SetCurSel(0);
}
