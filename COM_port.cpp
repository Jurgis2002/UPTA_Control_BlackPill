#include "pch.h"
#include "serialib2.h"

serialib2 serial;
CString  COM_port_name;

BOOL COM_port_Init()
{
  LPCSTR section = "General";
  COM_port_name = Utils_MainIni_GetString(section, "COM_port_name", "COM1");
  return COM_port_Init(COM_port_name);
}

BOOL COM_port_Init(LPCSTR port_name)
{
  DECLARE_MODULE_NAME;
  CString COM_port_name, main_ini;

  COM_port_name = port_name;  // Save port name globally
  DBG_LOG(COM_port_DebugLevel,"%s(): Open port \"%s\"\r\n", module, COM_port_name);

  int errorOpening = serial.openDevice(COM_port_name, 256000);
  CString s;
  if ( errorOpening == 1 ) {
    s = "Success";
    DBG_LOG(COM_port_DebugLevel, "%s(): Port \"%s\" %s\r\n", module, COM_port_name, s);
  }
  else {
    s = "Failed, error ";
    switch ( errorOpening ) {
    case -1: s.AppendFormat("%d - device not found",                       errorOpening); break;
    case -2: s.AppendFormat("%d - error while opening the device",         errorOpening); break;
    case -3: s.AppendFormat("%d - error while getting port parameters",    errorOpening); break;
    case -4: s.AppendFormat("%d - Speed (%d baud) not recognized",         errorOpening, 115200); break;
    case -5: s.AppendFormat("%d - error while writing port parameters",    errorOpening); break;
    case -6: s.AppendFormat("%d - error while writing timeout parameters", errorOpening); break;
    default: s.AppendFormat("%d - unknown",                                errorOpening); break;
    }
    DBG_LOG(1, "%s(): Port \"%s\" %s\r\n", module, COM_port_name, s);
  }
  
  {
    HANDLE hPort = HANDLE(serial);
    if( hPort != INVALID_HANDLE_VALUE ) {
      COMMPROP cp = { 0 };
      if( GetCommProperties(hPort, &cp) ) {
        DBG_LOG(COM_port_DebugLevel,"%s(): Max allowable baud rate 0x%08X\r\n", module, cp.dwMaxBaud);
        DBG_LOG(COM_port_DebugLevel,"%s(): Settable baud rate      %d\r\n", module, cp.dwSettableBaud);
      }
      // Show timeouts
      {
        COMMTIMEOUTS CommTimeouts = { 0 };
        if ( GetCommTimeouts(hPort, &CommTimeouts) ) {
          DBG_LOG(COM_port_DebugLevel, "%s(): ReadIntervalTimeout        = %d ms\r\n", module, CommTimeouts.ReadIntervalTimeout);
          DBG_LOG(COM_port_DebugLevel, "%s(): ReadTotalTimeoutMultiplier = %d\r\n", module, CommTimeouts.ReadTotalTimeoutMultiplier);
          DBG_LOG(COM_port_DebugLevel, "%s(): ReadTotalTimeoutConstant   = %d ms\r\n", module, CommTimeouts.ReadTotalTimeoutConstant);
        }
        else {
          DBG_LOG(1, "%s(): GetCommTimeouts() failed\r\n", module);
        }
      }
    }
  }
  return true;
}

void COM_port_Close()
{
  DECLARE_MODULE_NAME;

  DBG_LOG(COM_port_DebugLevel, "%s(): Entry\r\n", module);
  serial.closeDevice();
}

BOOL COM_port_IsOpened()
{
  DECLARE_MODULE_NAME;
  BOOL is_opened = HANDLE(serial) != INVALID_HANDLE_VALUE;

  DBG_LOG(COM_port_DebugLevel>1, "%s(): Port %s is %sopened\r\n", module, COM_port_name, (is_opened) ? "" : "not ");
  return is_opened;
}

BOOL COM_port_Flush()
{
  DECLARE_MODULE_NAME;

  DBG_LOG(COM_port_DebugLevel>1, "%s(): Entry\r\n", module);
  return serial.flushReceiver();
}

DWORD COM_port_CheckInQueue()
{
//return com_port.CheckInQueue();
  return serial.available();
}

BOOL  COM_port_GetByte(LPBYTE rByte, DWORD dwMilliseconds)
{
  DECLARE_MODULE_NAME;
  static BOOL first_msg = true;
  char   result;

//return com_port.GetByte(rByte, dwMilliseconds);
/*
  \return  1 success
  \return  0 Timeout reached
  \return -1 error while setting the Timeout
  \return -2 error while reading the byte
*/
  result = serial.readChar((char*)rByte, 5) == 1;
  if( result != 1 ) {
    DBG_LOG(first_msg, "%s(): Failed, error %s\r\n", module, serial.GetErrorStr(result));
    first_msg = false;
    return false;
  }
  first_msg = true;
  DBG_LOG(COM_port_DebugLevel > 1, "%s(): Byte read 0x%02X (%d)\r\n", module, *rByte, *rByte);
  return true;
}

BOOL COM_port_PutByte(BYTE wByte)
{
  DECLARE_MODULE_NAME;
  static BOOL first_msg = true;
  int    result;
/*
  \return  1 success
  \return -1 error while writting data
*/
  result = serial.writeChar(wByte);
  if( result != 1 ) {
    DBG_LOG(first_msg, "%s(): Failed, error %s\r\n", module, serial.GetErrorStr(result));
    first_msg = false;
    return false;
  }
  first_msg = true;
  DBG_LOG(COM_port_DebugLevel > 1, "%s(): Byte write 0x%02X (%d)\r\n", module, wByte, wByte);
  return true;
}

int COM_port_ReadBlock(LPBYTE lpBuffer, DWORD nMax, unsigned int timeOut_ms)
{
//return com_port.ReadBlock(lpBuffer, nMax);
/*
  \return >= 0 return the number of bytes read before timeout
               or requested data is completed
  \return -1 error while setting the Timeout
  \return -2 error while reading the byte
*/
  return serial.readBytes(lpBuffer, nMax, timeOut_ms);
}

BOOL COM_port_ReadLine(CString& line, const unsigned int timeout_ms)
{
  DECLARE_MODULE_NAME;
  char   receivedString[COM_PORT_MAX_STR_READ_LEN + 1] = { 0 };
  static BOOL first_msg = true;
  BOOL   result = false;
  /*
    \return > 0 success, return the number of bytes read
    \return  0 timeout is reached
    \return -1 error while setting the Timeout
    \return -2 error while reading the byte
    \return -3 MaxNbBytes is reached
  */
  if ( !COM_port_IsOpened() ) return false;  // we are not connected
  result = serial.readString(receivedString, '\n', COM_PORT_MAX_STR_READ_LEN, timeout_ms);
  if ( result > 0 ) {
    // Read string OK
    line = receivedString;
//  DBG_LOG(COM_port_DebugLevel > 1, "%s(): %d bytes read\r\n", module, result);
    DBG_LOG(COM_port_DebugLevel > 1, "%s(): string \"%s\"\r\n", module, line);
    first_msg = true;
    return true;
  }
  DBG_LOG(first_msg, "%s(): Failed, error %s\r\n", module, serial.GetErrorStr(result));
  first_msg = false;
  return false;
}

BOOL COM_port_WriteBlock(const void *Buffer, const unsigned int NbBytes)
{
  DECLARE_MODULE_NAME;
  static BOOL first_msg = true;
  int    result;
  /*
  \return 1 success
  \return -1 error while writting data
  */
  result = serial.writeBytes(Buffer, NbBytes);
  if ( result != 1 ) {
    DBG_LOG(first_msg, "%s(): Failed, error %s\r\n", module, serial.GetErrorStr(result));
    first_msg = false;
    return false;
  }
  first_msg = true;
  DBG_LOG(COM_port_DebugLevel > 1, "%s(): Write %d bytes success\r\n", module, NbBytes);
  return true;
}
