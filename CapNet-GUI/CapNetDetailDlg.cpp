#include "CapNetDetailDlg.h"
#include "CapNetStream.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CapNetDetailDlg, CDialogEx)
END_MESSAGE_MAP()


CapNetDetailDlg::CapNetDetailDlg(UINT packId)
	:
	CDialogEx(IDD_DETAIL_DIALOG),
	m_packId(packId),
	m_rawDataPool(this),
	m_stringData(this)
{
}

void CapNetDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_rawDataPool);
	DDX_Control(pDX, IDC_EDIT1, m_stringData);
}

BOOL CapNetDetailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CapNetOutStreamW woss;
	woss << woss.Format(L"No.%d ±¨ÎÄÏêÇé", m_packId);
	SetWindowTextW(woss.str().c_str());
	m_rawDataPool.Init();
	m_stringData.Init();
	return TRUE;
}
