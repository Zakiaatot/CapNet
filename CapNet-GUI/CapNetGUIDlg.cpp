#include <afxdialogex.h>
#include <afxbutton.h>
#include "CapNetGUI.h"
#include "CapNetGUIDlg.h"
#include "CapNetAboutDlg.h"
#include "CapNet-Core/CapNetCore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCapNetGUIDlg* CCapNetGUIDlg::m_pInstance = NULL;

BEGIN_MESSAGE_MAP(CCapNetGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

CCapNetGUIDlg::CCapNetGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CAPNETGUI_DIALOG, pParent),
	m_pacPool(this),
	m_devSelector(this),
	m_ruleEdit(this),
	m_listenButton(this),
	m_info(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
}

HWND CCapNetGUIDlg::GetHwnd()
{
	if (m_pInstance)
	{
		return m_pInstance->GetSafeHwnd();
	}
	return 0;
}

void CCapNetGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_devSelector);
	DDX_Control(pDX, IDC_EDIT1, m_ruleEdit);
	DDX_Control(pDX, IDC_LIST3, m_pacPool);
	DDX_Control(pDX, IDC_BUTTON2, m_listenButton);
	DDX_Control(pDX, IDC_TEXT, m_info);
}

BOOL CCapNetGUIDlg::OnInitDialog()
{
	m_pInstance = this;
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MINIMIZE);

	// Dev Select
	m_devSelector.Init();
	// Pac Pool
	m_pacPool.Init();
	// Listen Button
	m_listenButton.Init();
	// Rule Edit
	m_ruleEdit.Init();
	// Info
	m_info.Init();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCapNetGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CapNetAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CCapNetGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCapNetGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

