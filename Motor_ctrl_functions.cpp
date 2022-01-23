#include "pch.h"

static void send_command(LPCSTR module, LPCSTR s, CString& s_ret);

SUPTA_Defines  UPTA_Defines;
SMotor_control Motor_control;

void Read_HW_info(CString& info)
{
  CString s, s2, build_str, date_str, time_str;

  s = UPTA_Control_Command("V\r\n", true, 50);
  build_str = s;
  s = UPTA_Control_Command("VD\r\n", true, 50);
  date_str = s;
  s = UPTA_Control_Command("VT\r\n", true, 50);
  time_str = s;
  s2.Format("\"%s\", date \"%s\", time \"%s\"", build_str, date_str, time_str);
  // Short string
  info = build_str;
}

void Motor_Set_Run_speed(int ctrl_ID, int motor_index, int speed_hz)
{
  DECLARE_MODULE_NAME;
  CString s, s_ret;

  s.Format("F %d %d", motor_index, speed_hz);
  send_command(module, s, s_ret);
}

void Motor_Set_Direction(int motor_index, int dir)
{
  DECLARE_MODULE_NAME;
  CString s, s_ret;

  s.Format("D %d %d", motor_index, dir);
  send_command(module, s, s_ret);
}

void Motor_Set_Microstep(int motor_index, int microstep)
{
  DECLARE_MODULE_NAME;
  CString s, s_ret;

  s.Format("M %d %d", motor_index, microstep);
  send_command(module, s, s_ret);
}

void Motor_Set_Acc_Dec_params(char sub_cmd, int motor_index, int ctrl_ID1, int ctrl_ID2, int param1, int param2)
{
  DECLARE_MODULE_NAME;
  CString s, s_ret;

  s.Format("A%c %d %d %d", sub_cmd, motor_index, param1, param2);
  send_command(module, s, s_ret);
}

void Motor_Set_current_proc(int ctrl_ID, int motor_index, double current_proc, BOOL stopped)
{
  DECLARE_MODULE_NAME;
  CString s, s_ret;

  current_proc = check_and_correct(current_proc, 0.0, 100.0);
  SET_ITEM_FLOAT(ctrl_ID, current_proc);
  s.Format("U%c %d %.1f", (stopped) ? 'S' : 'R', motor_index, current_proc);
  send_command(module, s, s_ret);
}

//-------------------------------------------------------------------------------------------------
// Internals

static void send_command(LPCSTR module, LPCSTR s, CString& s_ret)
{
  s_ret = UPTA_Control_Command(s, true, 200);
  DEBUG_LOG("%s(): Ret string: \"%s\"\r\n", module, s_ret);
}
