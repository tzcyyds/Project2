// CDlgMessage.cpp: 实现文件
//

#include "pch.h"
#include "Chatser.h"
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
}


BEGIN_MESSAGE_MAP(CDlgMessage, CDialogEx)
END_MESSAGE_MAP()


// CDlgMessage 消息处理程序
