#ifndef COM_PORT_H
#define COM_PORT_H

#define COM_PORT_MAX_STR_READ_LEN   (512)

extern CString COM_port_name;

BOOL  COM_port_Init();
BOOL  COM_port_Init(LPCSTR port_name);
void  COM_port_Close();
BOOL  COM_port_IsOpened();
BOOL  COM_port_Flush();
DWORD COM_port_CheckInQueue();
BOOL  COM_port_GetByte(LPBYTE rByte, DWORD dwMilliseconds);
BOOL  COM_port_PutByte(BYTE wByte);
int   COM_port_ReadBlock(LPBYTE lpBuffer, DWORD nMax, unsigned int timeOut_ms);
BOOL  COM_port_ReadLine(CString& line, const unsigned int timeout_ms);
BOOL  COM_port_WriteBlock(const void *Buffer, const unsigned int NbBytes);

#endif // COM_PORT_H
