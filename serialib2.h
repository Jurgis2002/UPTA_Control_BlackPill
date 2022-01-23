/*!
\file    serialib.h
\brief   Header file of the class serialib. This class is used for communication over a serial device.
\author  Philippe Lucidarme (University of Angers)
\version 2.0
\date    december the 27th of 2019
This Serial library is used to communicate through serial port.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is a licence-free software, it can be used by anyone who try to build a better world.
*/
#ifndef SERIALIB2_H
#define SERIALIB2_H
#include <time.h>

// Include for windows
#if defined (_WIN32) || defined (_WIN64)
  // Accessing to the serial port under Windows
  #include <windows.h>
#endif

//*****************************************************************************
//  Helper class timeOut
//*****************************************************************************
#include "Z:\NetCommon\SCUSB_Common\Utils.h"
/*!
    \class     timeOut
    \brief     This class can manage a timer which is used as a timeout.
*/
class timeOut
{
public:
  // Constructor
  timeOut() { _time_prev_sec = 0; }

  // Init the timer
  void initTimer()
  {
    //  gettimeofday(&previousTime, NULL);
    _time_prev_sec = Utils_ExactTimer_GetElapsedSeconds();
  }

  // Return the elapsed time since initialization
  unsigned long int elapsedTime_ms()
  {
    double time_sec = Utils_ExactTimer_GetElapsedSeconds() - _time_prev_sec;
    return unsigned long int(time_sec * 1000.0);
  }

private:
  // Used to store the previous time (for computing timeout)
  double _time_prev_sec;
};

//*****************************************************************************
//  Class serialib2
//*****************************************************************************
/*!  \class     serialib2
     \brief     This class is used for communication over a serial device.
*/
class serialib2
{
public:
  //_____________________________________
  // ::: Constructors and destructors :::
  // Constructor of the class
  serialib2()
  {
    ZeroMemory(&timeouts, sizeof(timeouts));
    hSerial = INVALID_HANDLE_VALUE;
  }
  // Destructor
  ~serialib2() { closeDevice(); }

  //_________________________________________
  // ::: Configuration and initialization :::
  // Open a device
/*!
    \brief Open the serial port
    \param Device : Port name (COM1, COM2, ... for Windows )
    \param Bauds : Baud rate of the serial port.

    \return  1 success
    \return -1 device not found
    \return -2 error while opening the device
    \return -3 error while getting port parameters
    \return -4 Speed (Bauds) not recognized
    \return -5 error while writing port parameters
    \return -6 error while writing timeout parameters
*/
  int openDevice(const char *Device, const unsigned int Bauds)
  {
    // Open serial port
    CString port_name;
    port_name.Format("\\\\.\\%s", Device);
    hSerial = CreateFileA(
      //    Device,
      port_name,
      GENERIC_READ | GENERIC_WRITE,
      0,
      0,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      0
    );
    if (hSerial == INVALID_HANDLE_VALUE) {
      if (GetLastError() == ERROR_FILE_NOT_FOUND)
        return -1; // Device not found
      // Error while opening the device
      return -2;
    }
    // Set parameters

    // Structure for the port parameters
    DCB dcbSerialParams;
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    // Get the port parameters
    if ( !GetCommState(hSerial, &dcbSerialParams) ) return -3;

    // Set the speed (Bauds)
    switch (Bauds)
    {
    case 110:     dcbSerialParams.BaudRate = CBR_110; break;
    case 300:     dcbSerialParams.BaudRate = CBR_300; break;
    case 600:     dcbSerialParams.BaudRate = CBR_600; break;
    case 1200:     dcbSerialParams.BaudRate = CBR_1200; break;
    case 2400:     dcbSerialParams.BaudRate = CBR_2400; break;
    case 4800:     dcbSerialParams.BaudRate = CBR_4800; break;
    case 9600:     dcbSerialParams.BaudRate = CBR_9600; break;
    case 14400:    dcbSerialParams.BaudRate = CBR_14400; break;
    case 19200:    dcbSerialParams.BaudRate = CBR_19200; break;
    case 38400:    dcbSerialParams.BaudRate = CBR_38400; break;
    case 56000:    dcbSerialParams.BaudRate = CBR_56000; break;
    case 57600:    dcbSerialParams.BaudRate = CBR_57600; break;
    case 115200:   dcbSerialParams.BaudRate = CBR_115200; break;
    case 128000:   dcbSerialParams.BaudRate = CBR_128000; break;
    case 256000:   dcbSerialParams.BaudRate = CBR_256000; break;
    default: return -4;
    }
    // 8 bit data
    dcbSerialParams.ByteSize = 8;
    // One stop bit
    dcbSerialParams.StopBits = ONESTOPBIT;
    // No parity
    dcbSerialParams.Parity = NOPARITY;

    // Write the parameters
    if (!SetCommState(hSerial, &dcbSerialParams)) return -5;

    // Set TimeOut

    // Set the Timeout parameters
    timeouts.ReadIntervalTimeout = 0;
    // No TimeOut
    timeouts.ReadTotalTimeoutConstant = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = MAXDWORD;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    // Write the parameters
    if ( !SetCommTimeouts(hSerial, &timeouts) ) return -6;

    // Opening successfull
    return 1;
  }
/*!
      \brief Close the connection with the current device
*/
  void closeDevice()
  {
    if (hSerial != INVALID_HANDLE_VALUE) {
      PurgeComm(hSerial, PURGE_TXABORT | PURGE_RXABORT |
        PURGE_TXCLEAR | PURGE_RXCLEAR); // Clean up COM port
      CloseHandle(hSerial);
      hSerial = INVALID_HANDLE_VALUE;
    }
  }

  //___________________________________________
  // ::: Read/Write operation on characters :::
/*!
    \brief Write a char on the current serial port
    \param Byte : char to send on the port (must be terminated by '\0')
    \return 1 success
    \return -1 error while writting data
*/
  int writeChar(const char Byte)
  {
    if (hSerial == INVALID_HANDLE_VALUE) {
      return -1;
    }
    // Number of bytes written
    DWORD dwBytesWritten;
    // Write the char to the serial device
    // Return -1 if an error occured
    if ( !WriteFile(hSerial, &Byte, 1, &dwBytesWritten, NULL) ) return -1;
    // Write operation successfull
    return 1;
  }
  // Read a char (with timeout)
/*!
    \brief Wait for a byte from the serial device and return the data read
    \param pByte : data read on the serial device
    \param timeOut_ms : delay of timeout before giving up the reading
          If set to zero, timeout is disable (Optional)
    \return 1 success
    \return 0 Timeout reached
    \return -1 error while setting the Timeout
    \return -2 error while reading the byte
*/
  int readChar(char *pByte, const unsigned int timeOut_ms = 0)
  {
    if (hSerial == INVALID_HANDLE_VALUE) {
      return -2;
    }
    // Number of bytes read
    DWORD dwBytesRead = 0;

    // Set the TimeOut
    timeouts.ReadTotalTimeoutConstant = timeOut_ms;

    // Write the parameters, return -1 if an error occured
    if ( !SetCommTimeouts(hSerial, &timeouts) ) return -1;

    // Read the byte, return -2 if an error occured
    if ( !ReadFile(hSerial, pByte, 1, &dwBytesRead, NULL) ) return -2;

    // Return 0 if the timeout is reached
    if ( dwBytesRead == 0 ) return 0;

    // The byte is read
    return 1;
  }

  //________________________________________
  // ::: Read/Write operation on strings :::
/*!
     \brief     Write a string on the current serial port
     \param     receivedString : string to send on the port (must be terminated by '\0')
     \return     1 success
     \return    -1 error while writting data
*/
  int writeString(const char *String)
  {
    // Number of bytes written
    DWORD dwBytesWritten;

    // Write the string
    if (!WriteFile(hSerial, String, strlen(String), &dwBytesWritten, NULL))
      // Error while writing, return -1
      return -1;
    // Write operation successfull
    return 1;
  }
/*!
    \brief Read a string from the serial device (with timeout)
    \param receivedString : string read on the serial device
    \param finalChar : final char of the string
    \param maxNbBytes : maximum allowed number of bytes read
    \param timeOut_ms : delay of timeout before giving up the reading (optional)
    \return  > 0 success, return the number of bytes read
    \return  0 timeout is reached
    \return -1 error while setting the Timeout
    \return -2 error while reading the byte
    \return -3 MaxNbBytes is reached
*/
  int readString(char *receivedString, char finalChar, unsigned int maxNbBytes, const unsigned int timeOut_ms = 0)
  {
    // Check if timeout is requested
    if (timeOut_ms == 0) return readStringNoTimeOut(receivedString, finalChar, maxNbBytes);

    // Number of bytes read
    unsigned int    nbBytes = 0;
    // Character read on serial device
    char            charRead;
    // Timer used for timeout
    timeOut         timer;
    long int        timeOutParam;

    // Initialize the timer (for timeout)
    timer.initTimer();

    // While the buffer is not full
    while (nbBytes < maxNbBytes)
    {
      // Compute the TimeOut for the next call of ReadChar
      timeOutParam = timeOut_ms - timer.elapsedTime_ms();

      // If there is time remaining
      if (timeOutParam > 0)
      {
        // Wait for a byte on the serial link with the remaining time as timeout
        charRead = readChar(&receivedString[nbBytes], timeOutParam);

        // If a byte has been received
        if (charRead == 1)
        {
          // Check if the character received is the final one
          if (receivedString[nbBytes] == finalChar)
          {
            // Final character: add the end character 0
            receivedString[++nbBytes] = 0;
            // Return the number of bytes read
            return nbBytes;
          }
          // This is not the final character, just increase the number of bytes read
          nbBytes++;
        }
        // Check if an error occured during reading char
        // If an error occurend, return the error number
        if (charRead < 0) return charRead;
      }
      // Check if timeout is reached
      if (timer.elapsedTime_ms() > timeOut_ms)
      {
        // Add the end caracter
        receivedString[nbBytes] = 0;
        // Return 0 (timeout reached)
        return 0;
      }
    }

    // Buffer is full : return -3
    return -3;
  }

  // _____________________________________
  // ::: Read/Write operation on bytes :::
  // Write an array of bytes
/*!
    \brief Write an array of data on the current serial port
    \param Buffer : array of bytes to send on the port
    \param NbBytes : number of byte to send
    \return 1 success
    \return -1 error while writting data
*/
  int writeBytes(const void *Buffer, const unsigned int NbBytes)
  {
    // Number of bytes written
    DWORD dwBytesWritten;

    // Write data
    if ( !WriteFile(hSerial, Buffer, NbBytes, &dwBytesWritten, NULL) )
      // Error while writing, return -1
      return -1;
    // Write operation successfull
    return 1;
  }
  // Read an array of byte (with timeout)
/*!
    \brief Read an array of bytes from the serial device (with timeout)
    \param buffer : array of bytes read from the serial device
    \param maxNbBytes : maximum allowed number of bytes read
    \param timeOut_ms : delay of timeout before giving up the reading
    \param sleepDuration_us : delay of CPU relaxing in microseconds (Linux only)
          In the reading loop, a sleep can be performed after each reading
          This allows CPU to perform other tasks
    \return >=0 return the number of bytes read before timeout or
              requested data is completed
    \return -1 error while setting the Timeout
    \return -2 error while reading the byte
*/
  int readBytes(void *buffer, unsigned int maxNbBytes, const unsigned int timeOut_ms = 0)
  {
    if ( hSerial == INVALID_HANDLE_VALUE ) return -2;

    // Number of bytes read
    DWORD dwBytesRead = 0;

    // Set the TimeOut
    timeouts.ReadTotalTimeoutConstant = (DWORD)timeOut_ms;

    // Write the parameters and return -1 if an error occrured
    if ( !SetCommTimeouts(hSerial, &timeouts) ) return -1;


    // Read the bytes from the serial device, return -2 if an error occured
    if ( !ReadFile(hSerial, buffer, (DWORD)maxNbBytes, &dwBytesRead, NULL) ) return -2;

    // Return the byte read
    return dwBytesRead;
  }

  // _________________________
  // ::: Special operation :::
  // Empty the received buffer
/*!
    \brief Empty receiver buffer
    \return If the function succeeds, the return value is nonzero.
            If the function fails, the return value is zero.
*/
  BOOL flushReceiver()
  {
    // Purge receiver
    if (hSerial != INVALID_HANDLE_VALUE) {
      return PurgeComm(hSerial, PURGE_RXCLEAR);
    }
    else {
      return true;
    }
  }
  // Return the number of bytes in the received buffer
/*!
    \brief  Return the number of bytes in the received buffer (UNIX only)
    \return The number of bytes received by the serial provider but not yet read.
*/
  DWORD available()
  {
    if (hSerial != INVALID_HANDLE_VALUE) {
      // Device errors
      DWORD commErrors;
      // Device status
      COMSTAT commStatus;
      // Read status
      ClearCommError(hSerial, &commErrors, &commStatus);
      // Return the number of pending bytes
      return commStatus.cbInQue;
    }
    else {
      return 0;
    }
  }
  operator HANDLE () { return hSerial; }

  const char* GetErrorStr( int error )
  {
    switch ( error ) {
    case  1: return "success";                                     break;
    case  0: return " 0 - timeout reached";                        break;
    case -1: return "-1 - device not found";                       break;
    case -2: return "-2 - error while opening the device";         break;
    case -3: return "-3 - error while getting port parameters";    break;
    case -4: return "-4 - baud rate not recognized";               break;
    case -5: return "-5 - error while writing port parameters";    break;
    case -6: return "-6 - error while writing timeout parameters"; break;
    default: return " unknown";                                    break;
    }
  }

private:
  // Read a string (no timeout)
  int readStringNoTimeOut(char *receivedString, char finalChar, unsigned int maxNbBytes)
  {
    // Number of characters read
    unsigned int NbBytes = 0;
    // Returned value from Read
    char         charRead;

    // While the buffer is not full
    while (NbBytes < maxNbBytes) {
      // Read a character with the restant time
      charRead = readChar( &receivedString[NbBytes] );
      // Check a character has been read
      if (charRead == 1) {
        // Check if this is the final char
        if (receivedString[NbBytes] == finalChar) {
          // This is the final char, add zero (end of string)
          receivedString[++NbBytes] = 0;
          // Return the number of bytes read
          return NbBytes;
        }

        // The character is not the final char, increase the number of bytes read
        NbBytes++;
      }

      // An error occured while reading, return the error number
      if (charRead < 0) return charRead;
    }
    // Buffer is full : return -3
    return -3;
  }
  // Handle on serial device
  HANDLE       hSerial;
  // For setting serial port timeouts
  COMMTIMEOUTS timeouts;
};

#endif // SERIALIB2_H
