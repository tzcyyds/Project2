
// ChatserDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Chatser.h"
#include "ChatserDlg.h"
#include "afxdialogex.h"
#include "CDlgMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CChatserDlg 对话框



CChatserDlg::CChatserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATSER_DIALOG, pParent)
	, m_ip(0x7f000001)
	, m_port_local(9190)
	, m_port_remote(9191)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CChatserDlg::~CChatserDlg()
{

}

void CChatserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Text(pDX, IDC_EDIT1, m_port_local);
	DDX_Text(pDX, IDC_EDIT4, m_port_remote);
	DDX_Control(pDX, IDC_EDIT4, con_port_remote);
	DDX_Control(pDX, IDC_IPADDRESS1, con_ip);
}

BEGIN_MESSAGE_MAP(CChatserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CChatserDlg::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CChatserDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CChatserDlg 消息处理程序

BOOL CChatserDlg::OnInitDialog()
{
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

	// TODO: 在此添加额外的初始化代码
	m_combo.AddString("TCP");
	m_combo.AddString("UDP");
	m_combo.SetCurSel(0);
	OnCbnSelchangeCombo1();
	SetWindowText("Server");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CChatserDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CChatserDlg::OnPaint()
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
HCURSOR CChatserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CChatserDlg::OnBnClickedButton1()// 启动连接按钮
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CDlgMessage m_window(m_ip, m_port_local, m_port_remote, m_combo.GetCurSel());
	UpdateData(FALSE);
	EndDialog(0);// 关闭窗口
	m_window.DoModal();
}


void CChatserDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	BOOL is_udp = m_combo.GetCurSel();
	if (is_udp)
	{
		con_port_remote.EnableWindow(TRUE);
		con_ip.EnableWindow(TRUE);
	}
	else
	{
		con_port_remote.EnableWindow(FALSE);
		con_ip.EnableWindow(FALSE);
	}
}
