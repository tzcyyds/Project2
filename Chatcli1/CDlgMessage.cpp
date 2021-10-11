// CDlgMessage.cpp: 实现文件
//

#include "pch.h"
#include "Chatcli1.h"
#include "CDlgMessage.h"
#include "afxdialogex.h"


// CDlgMessage 对话框

IMPLEMENT_DYNAMIC(CDlgMessage, CDialogEx)

CDlgMessage::CDlgMessage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MESSAGE, pParent)
{

}

CDlgMessage::~CDlgMessage()
{
}

void CDlgMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_log);
	DDX_Control(pDX, IDC_EDIT1, m_send);
}


BEGIN_MESSAGE_MAP(CDlgMessage, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgMessage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgMessage::OnBnClickedButton2)
END_MESSAGE_MAP()


// CDlgMessage 消息处理程序


void CDlgMessage::OnBnClickedButton1()
{
	// 发送
}


void CDlgMessage::OnBnClickedButton2()
{
	// 返回
}
