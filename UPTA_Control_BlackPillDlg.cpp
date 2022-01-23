// UPTA_Control_BlackPillDlg.cpp : implementation file
//
#include "pch.h"
#include "framework.h"
#include "UPTA_Control_BlackPill.h"
#include "UPTA_Control_BlackPillDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char Program_file[MAX_PATH];
CString Version_num;

// CUPTAControlBlackPillDlg dialog

CUPTAControlBlackPillDlg::CUPTAControlBlackPillDlg(CWnd* pParent /*=nullptr*/)
  : CDialogEx(IDD_UPTA_CONTROL_BLACKPILL_DIALOG, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUPTAControlBlackPillDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUPTAControlBlackPillDlg, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CUPTAControlBlackPillDlg message handlers

BOOL CUPTAControlBlackPillDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);      // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon

  // TODO: Add extra initialization here

  // Set main program name
  GetModuleFileName(0, Program_file, MAX_PATH);
  strcpy_s(Program_file, MAX_PATH, PathFindFileName(Program_file));

  // Check for first instance of our application
  m_hMutex = CreateMutex(NULL, TRUE, "UPTA_Control_is_Running");
  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    CloseHandle(m_hMutex);
    Utils_MessageBox(NULL, "\n     Error: UPTA_Control program already running!     ", "OnInitDialog", INFINITE);
    EndDialog(IDCANCEL);
    return FALSE;
  }
  // Stop screensaver
  SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED); // Stop screen saver

  // Start program
  Debug_Levels();

  // Set main window header
  Version_num.Format("UPTA_Control [Ver %d.%d.%02d]",
    PROGRAM_VERSION_MAJOR, PROGRAM_VERSION_MINOR, PROGRAM_VERSION_MINOR2);
  SetWindowText(Version_num);

  COM_port_Init();
  // Read HW info
  {
    CString s;
    Read_HW_info(s);
    DEBUG_LOG("Ret string: %s\r\n", s);
    s.Format("HW version: %s", s);
    SET_ITEM_TEXT(IDC_STATIC_HW_version, s);
  }

  m_Timer1 = SetTimer(ID_TIMER_1, 50, NULL);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUPTAControlBlackPillDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
  COM_port_Close();
}

BOOL CUPTAControlBlackPillDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
  if (pMsg->message == WM_KEYDOWN) {
    CWnd* wnd = FromHandle(pMsg->hwnd);
    int ctrlID = wnd->GetDlgCtrlID();
    if (pMsg->wParam == VK_ESCAPE) {	// Block ESC key
      return TRUE;
    }
    if (pMsg->wParam == VK_RETURN) {	// Block RETURN key
      int ctrl_ID = wnd->GetDlgCtrlID();
      ////// Motor run speed, Hz
      ////if ( ctrl_ID == IDC_EDIT_M1_Speed_Hz || IDC_EDIT_M2_Speed_Hz )
      ////{
      ////  run_speed(ctrl_ID);
      ////}
      return TRUE;
    }
  }
	return CDialogEx::PreTranslateMessage(pMsg);
}
