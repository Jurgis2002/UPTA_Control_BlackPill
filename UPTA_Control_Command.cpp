//#include "stdafx.h"
#include "pch.h"

static void dump_data(BYTE* p_data, int data_len);

char Command_return_string[RET_STRING_LEN + 1];

LPCSTR UPTA_Control_Command(LPCSTR out_str, BOOL remove_crlf, int timeout_msec)
{
  DECLARE_MODULE_NAME;

  ZeroMemory(Command_return_string, sizeof(Command_return_string));
  if (COM_port_IsOpened()) {
    CString s_out, s_in;

    // com_port.Flush(); Too slow
    { // Measure com_port flush time
      double start = Utils_ExactTimer_GetElapsedSeconds();
      COM_port_Flush();
      double stop = Utils_ExactTimer_GetElapsedSeconds();
      DBG_LOG(COM_port_DebugLevel>1, "COM_port_Flush() time is %f sec\r\n", stop - start);
    }
    s_out = out_str;
    s_out.Trim();
    DBG_LOG(COM_port_DebugLevel, "%s: Out str \"%s\"\r\n", module, s_out);
    {
      CString s = s_out + "\r\n";
      COM_port_WriteBlock((LPBYTE)s.GetBuffer(), s.GetLength());
      DBG_LOG(COM_port_DebugLevel, "%s: Number of bytes written %d\r\n", module, s.GetLength());
    }
    s_in = "";
//  BOOL result = COM_port_ReadLine(s_in, timeout_msec / 1000.0); // !!! Timeout in seconds !!!
    BOOL result = COM_port_ReadLine(s_in, timeout_msec);
    DBG_LOG(COM_port_DebugLevel, "%s: Number of bytes read %d\r\n", module, s_in.GetLength());
    if (COM_port_DebugLevel > 1) {
      DBG_LOG(1, "%s: Number of bytes read %d\r\n", module, s_in.GetLength());
	    DBG_LOG(1, "Dump:\r\n");
	    dump_data((PUCHAR)s_in.GetBuffer(), s_in.GetLength());
    }
    if (!result && timeout_msec > 0) {
      _tcsncpy_s(Command_return_string, "TIMEOUT", _TRUNCATE);
      return Command_return_string; // Timeout return
    }
    _tcsncpy_s(Command_return_string, s_in.GetBuffer(), _TRUNCATE);

    if (remove_crlf) {
      char* p;
      p = strchr(Command_return_string, '\r');
      if (p) *p = '\0';
      p = strchr(Command_return_string, '\n');
      if (p) *p = '\0';
    }
    s_in = Command_return_string;
    s_in.Trim();
    DBG_LOG(COM_port_DebugLevel, "%s:  In str \"%s\"\r\n", module, s_in);

    s_in.MakeUpper();
    //if (s_in.Find("ERR") != -1) {
    //  DBG_LOG(1, "%s: Motor control error\r\n", module);
    //  DBG_LOG(1, "%s: Error send str \"%s\"\r\n", module, s_out);
    //  DBG_LOG(1, "%s: Error recv str \"%s\"\r\n", module, s_in);
    //  error_message(s_out, s_in, Command_return_string);
    //}
  } // if( com_port.GetOpenedStatus() )
  return Command_return_string;
}

BOOL UPTA_Control_send_command(LPCSTR module,CString &s)
{
//DECLARE_MODULE_NAME;
  BOOL result = true;
  CString ret_str;

  DEBUG_LOG( "%s(): Cmd string: \"%s\"\r\n", module, s.Trim("\r\n"));
  ret_str = UPTA_Control_Command(s.GetBuffer(), true, 200);
  DEBUG_LOG( "%s(): Ret string: \"%s\"\r\n", module, ret_str );
  if (ret_str.Find("ERR") != -1) {
    DBG_LOG(1, "Send command error, command \"%s\"\r\n", s);
    result = false;
  }
  s = ret_str;
  return result;
}

static void dump_data(BYTE* p_data, int data_len)
{
  int   data_size = 1;
  int   line_items = 8;
  CString s;

  // Dump data
  while (data_len) {
    s = "";
    int i = 0;
    while (data_len) {
      s.AppendFormat("0x%02X ", *((BYTE*)p_data));
      data_len--;
      p_data++;
      i++;
      if (i >= line_items) break;
    }
    s.AppendFormat("\r\n");
    Utils_protocol_str(s);
  }
}
