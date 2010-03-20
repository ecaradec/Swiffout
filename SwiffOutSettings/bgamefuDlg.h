// bgamefuDlg.h : header file
//

#pragma once

struct FlashWnd;
// CbgamefuDlg dialog
class CbgamefuDlg : public CDialog
{
// Construction
public:
	CbgamefuDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_BGAMEFU_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButton2();
    DECLARE_MESSAGE_MAP()
public:
public:
    DEVMODE dm[1000];

    CListBox m_resolutionList;
    CButton  m_apply;
public:
    afx_msg void OnBnClickedButton3();
};
