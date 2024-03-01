#include <afxdialogex.h>
#include "CapNet-GUI.h"
#include "CapNet-GUIDlg.h"
#include "CapNet-Core/CapNetCore.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCapNetGUIDlg* gPGuiObj = NULL;
UINT gPacListNum = 0;

static VOID ListenPack(CapNetCore::Pack& pack)
{
	gPGuiObj->m_pacPool.InsertItem(gPacListNum, L"");
	WCHAR buf[16] = { 0 };
	wsprintfW(buf, L"%u", pack.no);
	gPGuiObj->m_pacPool.SetItemText(gPacListNum, 0, buf);
	gPGuiObj->m_pacPool.SetItemText(gPacListNum, 1, pack.time.c_str());
	gPGuiObj->m_pacPool.SetItemText(gPacListNum, 2, pack.source.c_str());
	gPGuiObj->m_pacPool.SetItemText(gPacListNum, 3, pack.dest.c_str());
	gPGuiObj->m_pacPool.SetItemText(gPacListNum, 4, pack.protocol.c_str());
	wsprintfW(buf, L"%u", pack.length);
	gPGuiObj->m_pacPool.SetItemText(gPacListNum, 5, buf);
	gPGuiObj->m_pacPool.SetItemText(gPacListNum, 6, pack.info.c_str());
	gPacListNum++;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCapNetGUIDlg 对话框



CCapNetGUIDlg::CCapNetGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CAPNETGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
}

void CCapNetGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_devSelector);
	DDX_Control(pDX, IDC_EDIT1, m_ipEdit);
	DDX_Control(pDX, IDC_LIST3, m_pacPool);
	DDX_Control(pDX, IDC_COMBO2, m_pacType);
}

BEGIN_MESSAGE_MAP(CCapNetGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CCapNetGUIDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, &CCapNetGUIDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CCapNetGUIDlg 消息处理程序

BOOL CCapNetGUIDlg::OnInitDialog()
{
	gPGuiObj = this;
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
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MINIMIZE);

	// Dev Select
	CapNetCore::StatusVoid init = Singleton<CapNetCore>::Instance().Init();
	if (!init)
	{
		Utils::AlertErrorA(init.msg.c_str());
		Exit();
	}
	auto ret = Singleton<CapNetCore>::Instance().FindAllDev();
	if (!ret)
	{
		Utils::AlertErrorA(ret.msg.c_str());
		Exit();
	}
	else
	{
		for (auto i = ret.ret.begin(); i != ret.ret.end(); i++)
		{
			CA2W ca2w(i->second.c_str());
			std::wstring ws(ca2w);
			m_devSelector.AddString(ws.c_str());
		}
		m_devSelector.SetCurSel(0);
	}

	// Pac Type
	m_pacType.AddString(L"eth");
	m_pacType.AddString(L"ip");
	m_pacType.AddString(L"arp");
	m_pacType.AddString(L"tcp");
	m_pacType.AddString(L"udp");
	m_pacType.AddString(L"icmp");
	m_pacType.AddString(L"http");
	m_pacType.SetCurSel(0);

	// Pac Pool
	CRect rect;
	m_pacPool.GetClientRect(&rect);
	// 为列表视图控件添加三列   
	m_pacPool.InsertColumn(0, _T("No."), LVCFMT_LEFT, 80, 0);
	m_pacPool.InsertColumn(1, _T("Time"), LVCFMT_LEFT, 180, 1);
	m_pacPool.InsertColumn(2, _T("Source"), LVCFMT_LEFT, 250, 2);
	m_pacPool.InsertColumn(3, _T("Destination"), LVCFMT_LEFT, 250, 3);
	m_pacPool.InsertColumn(4, _T("Protocol"), LVCFMT_LEFT, 120, 4);
	m_pacPool.InsertColumn(5, _T("Length"), LVCFMT_LEFT, 100, 5);
	m_pacPool.InsertColumn(6, _T("Info"), LVCFMT_LEFT, 9999, 6);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCapNetGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
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



void CCapNetGUIDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	//int id = m_devSelector.GetCurSel();
	//Utils::AlertInfoA("Select %d", id);
}



void CCapNetGUIDlg::OnBnClickedButton1()
{
	CString ip;
	m_ipEdit.GetWindowTextW(ip);
	if (ip.GetLength() > 0 && !Utils::CheckIpAddressW(ip.GetString()))
	{
		Utils::AlertErrorW(L"请检查指定IP地址格式");
		return;
	}

	if (Singleton<CapNetCore>::Instance().BeginListen(
		m_devSelector.GetCurSel(),
		L"eth",
		L"",
		ListenPack
	))
	{
		Utils::AlertInfoW(L"开始监听");
	};
}
