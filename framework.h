#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

// TODO: reference additional headers your program requires here

#define DECLARE_MODULE_NAME   LPCTSTR module = __FUNCTION__

#include "Z:\NetCommon\SCUSB_Common\Utils.h"
#include "Z:\NetCommon\SCUSB_Common\UPTA_Defines.h"

#include "UPTA_Control_BlackPill.h"
#include "UPTA_Control_BlackPillDlg.h"

#include "Program_ver.h"
#include "resource.h"

#include "serialib2.h"
#include "COM_port.h"

/////////////////////////////////////////////////////////////////////////////////////////
//  UI definitions
//
#define MAIN_CWnd               theApp.m_pMainWnd
#define MAIN_Dlg                ((CUPTAControlBlackPillDlg*)theApp.m_pMainWnd)
#define MAIN_HWND               (MAIN_CWnd)->m_hWnd
#define GET_ITEM(id)            theApp.m_pMainWnd->GetDlgItem(id)
#define GET_ITEM_HWND(id)       theApp.m_pMainWnd->GetDlgItem(id)->m_hWnd

#define GET_ITEM_TEXT(id,t)     (GET_ITEM(id)->GetWindowText(t))
#define SET_ITEM_TEXT(id,t)     (GET_ITEM(id)->SetWindowText(t))

#define GET_ITEM_INT(id,ptr_i)      \
  {                                 \
    CString s;                      \
    GET_ITEM(id)->GetWindowText(s); \
    *ptr_i = atoi(s);               \
  }
#define SET_ITEM_INT(id,n) \
  {                        \
    CString s;             \
    s.Format("%d", n);     \
    SET_ITEM_TEXT(id, s);  \
  }

#define GET_ITEM_FLOAT(id,ptr_f)    \
  {                                 \
    CString s;                      \
    GET_ITEM(id)->GetWindowText(s); \
    *ptr_f = atof(s);               \
  }
#define SET_ITEM_FLOAT(id,n) \
  {                          \
    CString s;               \
    s.Format("%.3f", n);     \
    s.TrimRight('0');        \
    if(s.Right(1) == '.')    \
      s+= '0';               \
    SET_ITEM_TEXT(id, s);    \
  }

/////////////////////////////////////////////////////////////////////////////////////////
//  Globals
//
extern char Program_file[MAX_PATH];
extern CString Version_num;

/////////////////////////////////////////////////////////////////////////////////////////
//  File "Debug_levels.cpp"
//
extern UINT DebugLevel;
extern UINT COM_port_DebugLevel;

void Debug_Levels();

/////////////////////////////////////////////////////////////////////////////////////////
//  File "UPTA_Control_Command.cpp"
//
#define RET_STRING_LEN    64

LPCSTR UPTA_Control_Command(LPCSTR out_str, BOOL remove_crlf, int timeout_msec);
BOOL   UPTA_Control_send_command(LPCSTR module, CString &s);

/////////////////////////////////////////////////////////////////////////////////////////
//  File "Motor_ctrl_functions.cpp"
//
extern SUPTA_Defines  UPTA_Defines;
extern SMotor_control Motor_control;

void Read_HW_info(CString& info);
void Motor_Set_Run_speed(int ctrl_ID, int motor_index, int speed_hz);
void Motor_Set_Direction(int motor_index, int dir);
void Motor_Set_Microstep(int motor_index, int microstep);
void Motor_Set_Acc_Dec_params(char sub_cmd, int motor_index, int ctrl_ID1, int ctrl_ID2, int param1, int param2);
void Motor_Set_current_proc(int ctrl_ID, int motor_index, double current_proc, BOOL stopped);
