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
	DDX_Control(pDX, IDC_LIST1, m_recv);
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
	m_send.Empty();
	UpdateData(FALSE);
}


void CDlgMessage::OnBnClickedButton3()// 返回按钮
{
	// TODO: Add your control notification handler code here
}



void CDlgMessage::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CDlgMessage::OnEnChangeEdit3()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CDlgMessage::OnEnChangeEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
