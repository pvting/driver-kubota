#ifndef _serialPort_h
#define _serialPort_h

#include <fcntl.h>
#include <unistd.h>

#define printfLog
 
class serialPort { 
public :
    serialPort() {isPrintLog = false;}
    serialPort(char const* portName, unsigned int baudRate);
    virtual ~serialPort();
    void serialInitialization(char const* portName, unsigned int baudRate);
    int serialInitializationReturnError(char const* portName, unsigned int baudRate);
    void readData(unsigned char* buf, unsigned int bufSize);
    void writeData(unsigned char const* buf, unsigned int bufSize);
protected :
    int fd;
    bool isPrintLog;
    int fd_log;
};

#endif
