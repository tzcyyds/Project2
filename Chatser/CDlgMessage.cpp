﻿// CDlgMessage.cpp: 实现文件
//

#include "pch.h"
#include "Chatser.h"
#include "CDlgMessage.h"
#include "afxdialogex.h"

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
	memset(&clntAdr, 0, sizeof(clntAdr));
	clntAdrLen = sizeof(clntAdr);
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
}


BEGIN_MESSAGE_MAP(CDlgMessage, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgMessage::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgMessage::OnBnClickedButton3)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CDlgMessage::OnLvnItemchangedList1)
	ON_EN_CHANGE(IDC_EDIT3, &CDlgMessage::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT2, &CDlgMessage::OnEnChangeEdit2)
END_MESSAGE_MAP()


// CDlgMessage 消息处理程序

BOOL CDlgMessage::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText("Server");
	clntAdr.sin_family = AF_INET;
	clntAdr.sin_addr.s_addr = htonl(m_ip);
	clntAdr.sin_port = htons(m_port_remote);
	WSADATA wsaData;
	SOCKADDR_IN servAdr;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox("WSAStartup() failed", "Server", MB_OK);
		exit(1);
	}
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(m_port_local);
	if (is_udp)
	{
		hCommSock = socket(AF_INET, SOCK_DGRAM, 0);
		if (hCommSock == INVALID_SOCKET)
		{
			MessageBox("socket() failed", "Server", MB_OK);
			exit(1);
		}
		if (bind(hCommSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		{
			MessageBox("bind() failed", "Server", MB_OK);
			exit(1);
		}
		if (WSAAsyncSelect(hCommSock, m_hWnd, WM_SOCK, FD_READ | FD_CLOSE) == SOCKET_ERROR)
		{
			MessageBox("WSAAsyncSelect() failed", "Server", MB_OK);
			exit(1);
		}
	}
	else
	{
		SOCKET hListenSock;
		hListenSock = socket(AF_INET, SOCK_STREAM, 0);
		if (hListenSock == INVALID_SOCKET)
		{
			MessageBox("socket() failed", "Server", MB_OK);
			exit(1);
		}
		if (bind(hListenSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		{
			MessageBox("bind() failed", "Server", MB_OK);
			exit(1);
		}
		if (listen(hListenSock, 5) == SOCKET_ERROR)
		{
			MessageBox("listen() failed", "Server", MB_OK);
			exit(1);
		}
		if (WSAAsyncSelect(hListenSock, m_hWnd, WM_SOCK, FD_ACCEPT | FD_READ | FD_CLOSE) == SOCKET_ERROR)
		{
			MessageBox("WSAAsyncSelect() failed", "Server", MB_OK);
			exit(1);
		}
	}

	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE |
		CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;
	cf.dwEffects = CFE_UNDERLINE;
	cf.yHeight = 16 * 16;//文字高度
	cf.crTextColor = RGB(200, 100, 255); //文字颜色
	strcpy_s(cf.szFaceName, _T("隶书"));//设置字体
	m_RichEdit.SetDefaultCharFormat(cf);

	CString strText = "Hello, World! By Colin\r\n";
	m_RichEdit.SetWindowText(strText);

	return TRUE;
}


LRESULT CDlgMessage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	SOCKET hSocket;
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
		case FD_ACCEPT:
			hCommSock = accept(hSocket, (sockaddr*)&clntAdr, &clntAdrLen);
			if (hCommSock == SOCKET_ERROR)
			{
				closesocket(hSocket);
				break;
			}
			MessageBox("Connection Accepted", "Server", MB_OK);
			break;
		case FD_READ:
			//UpdateData(TRUE);
			if (is_udp)
			{
				strLen = recvfrom(hSocket, buf, MAX_BUF_SIZE, 0, (sockaddr*)&clntAdr, &clntAdrLen);
				if (strLen <= 0)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						closesocket(hSocket);
						MessageBox("recvfrom() failed", "Server", MB_OK);
						break;
					}
				}
				else
				{
					CString m_recv(buf);
					m_RichEdit.SetSel(-1, -1);
					updatetime();
					m_recv = "recv" + m_time + m_recv;
					m_RichEdit.ReplaceSel(m_recv);
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
						MessageBox("recv() failed", "Server", MB_OK);
						break;
					}
				}
				else
				{
					CString m_recv(buf);
					m_RichEdit.SetSel(-1, -1);
					updatetime();
					m_recv = "recv" + m_time + m_recv;
					m_RichEdit.ReplaceSel(m_recv);
				}
			}
			//buf[strLen] = 0;
			UpdateData(FALSE);// TODO:接收的控件处理，控件的句柄是m_recv
			break;
		case FD_CLOSE:
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
		sendto(hCommSock, m_send, strLen, 0, (sockaddr*)&clntAdr, clntAdrLen);
	}
	else
	{
		if (hCommSock == SOCKET_ERROR)
		{
			MessageBox("No Connected Client", "Server", MB_OK);
			return;
		}
		send(hCommSock, m_send, strLen, 0);
	}
	m_RichEdit.SetSel(-1, -1);
	updatetime();
	m_send = "send" + m_time + m_send;
	m_RichEdit.ReplaceSel(m_send);
	m_send.Empty();
	UpdateData(FALSE);
}


void CDlgMessage::OnBnClickedButton3()// 返回按钮
{
	// TODO: Add your control notification handler code here
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
	strftime(timE, 40, " %I:%M:%S > ", &timeinfo);
	//printf("%s", timE);
	m_time = timE;
}
