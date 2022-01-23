// UPTA_Control_BlackPillDlg.h : header file
//
#pragma once

#define ID_TIMER_1 100

// CUPTAControlBlackPillDlg dialog
class CUPTAControlBlackPillDlg : public CDialogEx
{
// Construction
public:
	CUPTAControlBlackPillDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UPTA_CONTROL_BLACKPILL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
  HANDLE m_hMutex;
  UINT_PTR m_Timer1;

	// Generated message map functions
	virtual BOOL OnInitDialog();
//	afx_msg void OnPaint();
//	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
