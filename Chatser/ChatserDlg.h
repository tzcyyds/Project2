
// ChatserDlg.h: 头文件
//

#pragma once


// CChatserDlg 对话框
class CChatserDlg : public CDialogEx
{
// 构造
public:
	CChatserDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CChatserDlg();// 析构函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATSER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
private:
	CComboBox m_combo;
	DWORD m_ip;
	UINT m_port_local;
	UINT m_port_remote;
public:
	afx_msg void OnCbnSelchangeCombo1();
private:
	CEdit con_port_remote;
	CIPAddressCtrl con_ip;
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR* pNMHDR, LRESULT* pResult);
};
