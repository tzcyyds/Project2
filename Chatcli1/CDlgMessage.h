#pragma once


// CDlgMessage 对话框

class CDlgMessage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMessage)

public:
	CDlgMessage(DWORD _m_ip, UINT _m_port_local, UINT _m_port_remote,
		BOOL _is_udp, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgMessage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MESSAGE };
#endif
	DWORD m_ip;
	UINT m_port_local;
	UINT m_port_remote;
	BOOL is_udp;
	SOCKET hCommSock;
	SOCKADDR_IN servAdr;
	int servAdrLen;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual LRESULT CDlgMessage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CString m_send;
	CListCtrl m_recv;
};
