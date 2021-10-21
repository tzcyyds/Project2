// CDlgMessage.cpp: 实现文件
//

#include "pch.h"
#include "Chatcli1.h"
#include "CDlgMessage.h"
#include "afxdialogex.h"
#include "Chatcli1Dlg.h"


#define WM_SOCK WM_USER + 1// 自定义消息，在WM_USER的基础上进行
#define MAX_BUF_SIZE 128


// CDlgMessage 对话框

IMPLEMENT_DYNAMIC(CDlgMessage, CDialogEx)

CDlgMessage::CDlgMessage(DWORD _m_ip, UINT _m_port_local, UINT _m_port_remote,
	BOOL _is_udp, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MESSAGE, pParent)
	, m_ip(_m_ip)
	, m_port_local(_m_port_local)
	, m_port_remote(_m_port_remote)
	, is_udp(_is_udp)
	, m_send(_T(""))
{
	hCommSock = 0;
	memset(&servAdr, 0, sizeof(servAdr));
	servAdrLen = sizeof(servAdr);
	updatetime();
}

CDlgMessage::~CDlgMessage()
{

}

void CDlgMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_send);
	//  DDX_Control(pDX, IDC_LIST1, m_recv);
	DDX_Control(pDX, IDC_RICHEDIT21, m_RichEdit);
	DDX_Control(pDX, IDC_EDIT2, m_remote_address);
	DDX_Control(pDX, IDC_BUTTON2, m_message_send);
}


BEGIN_MESSAGE_MAP(CDlgMessage, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgMessage::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgMessage::OnBnClickedButton3)
END_MESSAGE_MAP()


// CDlgMessage 消息处理程序

BOOL CDlgMessage::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText("Client");

	ModifyStyleEx(0, WS_EX_APPWINDOW);  // 添加任务栏
	ShowWindow(SW_SHOW);
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE |
		CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;
	// cf.dwEffects = CFE_UNDERLINE;
	cf.yHeight = 14 * 14;//文字高度
	cf.crTextColor = RGB(6, 128, 67); //文字颜色
	strcpy_s(cf.szFaceName, _T("隶书"));//设置字体
	m_RichEdit.SetDefaultCharFormat(cf);

	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(m_ip);
	servAdr.sin_port = htons(m_port_remote);

	m_remote_address.SetWindowTextA(inet_ntoa(servAdr.sin_addr));

	WSADATA wsaData;
	SOCKADDR_IN clntAdr;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox("WSAStartup() failed", "Client", MB_OK);
		exit(1);
	}
	memset(&clntAdr, 0, sizeof(clntAdr));
	clntAdr.sin_family = AF_INET;
	clntAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	clntAdr.sin_port = htons(m_port_local);
	if (is_udp)
	{
		hCommSock = socket(AF_INET, SOCK_DGRAM, 0);
		if (hCommSock == INVALID_SOCKET)
		{
			MessageBox("socket() failed", "Client", MB_OK);
			exit(1);
		}
		if (bind(hCommSock, (SOCKADDR*)&clntAdr, sizeof(clntAdr)) == SOCKET_ERROR)
		{
			MessageBox("bind() failed", "Client", MB_OK);
			exit(1);
		}
		if (WSAAsyncSelect(hCommSock, m_hWnd, WM_SOCK, FD_READ | FD_CLOSE) == SOCKET_ERROR)
		{
			MessageBox("WSAAsyncSelect() failed", "Client", MB_OK);
			exit(1);
		}
	}
	else
	{
		hCommSock = socket(AF_INET, SOCK_STREAM, 0);
		if (hCommSock == INVALID_SOCKET)
		{
			MessageBox("socket() failed", "Client", MB_OK);
			exit(1);
		}
		if (connect(hCommSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		{
			MessageBox("connect() failed", "Client", MB_OK);
			exit(1);
		}
		SendwithColor("Connection Accepted.\n", 6, 128, 67, 14);  // 自定义函数：设置发送文字颜色和大小并发送
		if (WSAAsyncSelect(hCommSock, m_hWnd, WM_SOCK, FD_READ | FD_CLOSE) == SOCKET_ERROR)
		{
			MessageBox("WSAAsyncSelect() failed", "Client", MB_OK);
			exit(1);
		}
	}

	return TRUE;
}


SOCKET hSocket;

LRESULT CDlgMessage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	char buf[MAX_BUF_SIZE] = { 0 };
	int strLen;
	int newEvent;
	switch (message)
	{
	case WM_SOCK:
		hSocket = (SOCKET)LOWORD(wParam);
		newEvent = LOWORD(lParam);
		switch (newEvent)
		{
		case FD_READ:
			//UpdateData(TRUE);
			if (is_udp)
			{
				strLen = recvfrom(hSocket, buf, MAX_BUF_SIZE, 0, (sockaddr*)&servAdr, &servAdrLen);
				if (strLen <= 0)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						closesocket(hSocket);
						//SendwithColor("Connection Canceled.\n", 6, 128, 67, 14);
						MessageBox("recvfrom() failed", "Client", MB_OK);
						break;
					}
				}
				else
				{
					CString m_recv(buf);
					updatetime();
					SendwithColor("server" + m_time, 30, 144, 255, 14);  // 自定义函数：设置发送文字颜色和大小并发送
					SendwithColor(m_recv, 0, 0, 0, 15);
				}
			}
			else
			{
				strLen = recv(hSocket, buf, MAX_BUF_SIZE, 0);
				if (strLen <= 0)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						closesocket(hSocket);
						//SendwithColor("Connection Canceled.\n", 6, 128, 67, 14);
						MessageBox("recv() failed", "Client", MB_OK);
						break;
					}
				}
				else
				{
					CString m_recv(buf);
					updatetime();
					SendwithColor("server" + m_time, 30, 144, 255, 14);  // 自定义函数：设置发送文字颜色和大小并发送
					SendwithColor(m_recv, 0, 0, 0, 15);
				}
			}
			//buf[strLen] = 0;
			UpdateData(FALSE);// TODO:接收的控件处理，控件的句柄是m_recv
			break;
		case FD_CLOSE:
			SendwithColor("Connection Canceled.\n", 254, 40, 14, 14);
			m_remote_address.SetWindowTextA(_T("无"));
			m_message_send.EnableWindow(FALSE);
			closesocket(hSocket);
			break;
		}
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}


void CDlgMessage::OnBnClickedButton2()// 发送按钮
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_send.AppendChar('\n');
	int strLen = m_send.GetLength();
	if (is_udp)
	{
		sendto(hCommSock, m_send, strLen, 0, (sockaddr*)&servAdr, servAdrLen);
	}
	else
	{
		send(hCommSock, m_send, strLen, 0);
	}
	//m_RichEdit.SetWindowText(m_send);
	updatetime();
	SendwithColor("client" + m_time, 6, 128, 67, 14);  // 自定义函数：设置发送文字颜色和大小并发送
	SendwithColor(m_send, 0, 0, 0, 15);
	m_send.Empty();
	UpdateData(FALSE);
}


void CDlgMessage::OnBnClickedButton3()// 返回按钮
{
	// TODO: Add your control notification handler code here
	//m_sure.EnableWindow(FALSE);
	//CDialogEx::ShowWindow(SW_SHOW);
	//UpdateData(TRUE);
	shutdown(hSocket, SD_BOTH);
	closesocket(hSocket);
	CChatcli1Dlg m_window(NULL);
	//UpdateData(FALSE);
	EndDialog(0);// 关闭窗口
	//DestroyWindow();
	//m_sure.EnableWindow(FALSE);
	//CDialogEx::ShowWindow(SW_HIDE);
	m_window.DoModal();
}


void CDlgMessage::updatetime()
{
	// TODO: 在此处添加实现代码.
	time_t rawtime;
	struct tm timeinfo;
	char timE[40] = { 0 };
	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);
	//strftime(timE, 40, "Date:\n%Y-%m-%d\nTime:\n%I:%M:%S\n", &timeinfo);
	strftime(timE, 40, " %Y-%m-%d %H:%M:%S\n", &timeinfo);
	//printf("%s", timE);
	m_time = timE;
}


void CDlgMessage::SendwithColor(CString s, int R, int G, int B, int size)  // 自定义发送文字的颜色、字体
{
	m_RichEdit.SetSel(-1, -1);

	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE |
		CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;
	// cf.dwEffects = CFE_UNDERLINE;
	cf.yHeight = size * size;//文字高度
	cf.crTextColor = RGB(R, G, B); //文字颜色
	strcpy_s(cf.szFaceName, _T("隶书"));//设置字体

	m_RichEdit.SetSelectionCharFormat(cf);
	m_RichEdit.ReplaceSel(s);
}
