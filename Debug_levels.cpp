//#include "stdafx.h"
#include "pch.h"

UINT DebugLevel = 0;
UINT COM_port_DebugLevel = 0;

void Debug_Levels()
{
  LPCSTR section = "Debug";

  DebugLevel = Utils_MainIni_GetLong(section, "DebugLevel", 0);
  COM_port_DebugLevel = Utils_DebugLevels_Get(Program_file, "COM_port_DebugLevel");
}
