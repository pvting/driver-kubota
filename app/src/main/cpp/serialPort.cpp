#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <sstream>
#include <sys/stat.h>
#include "serialPort.h"
#include "xcept.h"

serialPort::serialPort(char const* portName, unsigned int baudRate)
{
    struct termios Opt;
    speed_t s;

    fd = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd) {
        throw vendingMachineError("serial open error");
    }
    if (-1 == fcntl(fd, F_SETFL, 0)) {
        throw vendingMachineError("serial fcntl error");
    }
    if (tcgetattr(fd, &Opt) != 0) {
        throw vendingMachineError("serial tcgetattr error");
    }
    switch (baudRate) {
    case 115200 :
        s = B115200;
        break;
    case 38400 :
        s = B38400;
        break;
    case 19200 :
        s = B19200;
        break;
    default :
        throw vendingMachineError("serial baudRate error");
    }
    cfsetispeed(&Opt, s);
    cfsetospeed(&Opt, s);

	Opt.c_cflag &= ~CSTOPB;  // one stop bit
	Opt.c_cflag |= (CLOCAL | CREAD);

	// raw input/output
    cfmakeraw(&Opt);  // no parity, Mask the character size bits, 8 bit
    Opt.c_cflag |= PARENB;
    Opt.c_cflag &= ~PARODD;
    Opt.c_iflag |= (INPCK /*| ISTRIP*/);
	Opt.c_cc[VMIN] = 1;
	Opt.c_cc[VTIME] = 0;
    tcflush(fd, TCIOFLUSH);
    if (tcsetattr(fd, TCSANOW, &Opt) != 0)
    {
        throw vendingMachineError("serial tcsetattr error");
    }
}

serialPort::~serialPort()
{
    if (-1 == close(fd)) {
        throw vendingMachineError("serial close error");
    }
}

void serialPort::serialInitialization(char const* portName, unsigned int baudRate)
{
    struct termios Opt;
    speed_t s;

    fd = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd) {
        throw vendingMachineError("serial open error");
    }
    if (-1 == fcntl(fd, F_SETFL, 0)) {
        throw vendingMachineError("serial fcntl error");
    }
    if (tcgetattr(fd, &Opt) != 0) {
        throw vendingMachineError("serial tcgetattr error");
    }
    switch (baudRate) {
    case 115200 :
        s = B115200;
        break;
    case 38400 :
        s = B38400;
        break;
    case 19200 :
        s = B19200;
        break;
    default :
        throw vendingMachineError("serial baudRate error");
    }
    cfsetispeed(&Opt, s);
    cfsetospeed(&Opt, s);

	Opt.c_cflag &= ~CSTOPB;  // one stop bit
	Opt.c_cflag |= (CLOCAL | CREAD);

	// raw input/output
    cfmakeraw(&Opt);  // no parity, Mask the character size bits, 8 bit
    Opt.c_cflag |= PARENB;
    Opt.c_cflag &= ~PARODD;
    Opt.c_iflag |= (INPCK /*| ISTRIP*/);
	Opt.c_cc[VMIN] = 1;
	Opt.c_cc[VTIME] = 0;
    tcflush(fd, TCIOFLUSH);
    if (tcsetattr(fd, TCSANOW, &Opt) != 0)
    {
        throw vendingMachineError("serial tcsetattr error");
    }
}

int serialPort::serialInitializationReturnError(char const* portName, unsigned int baudRate)
{
    struct termios Opt;
    speed_t s;

    fd = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd) {
        return -1;
    }
    if (-1 == fcntl(fd, F_SETFL, 0)) {
        throw vendingMachineError("serial fcntl error");
    }
    if (tcgetattr(fd, &Opt) != 0) {
        throw vendingMachineError("serial tcgetattr error");
    }
    switch (baudRate) {
        case 115200 :
            s = B115200;
            break;
        case 38400 :
            s = B38400;
            break;
        case 19200 :
            s = B19200;
            break;
        default :
            throw vendingMachineError("serial baudRate error");
    }
    cfsetispeed(&Opt, s);
    cfsetospeed(&Opt, s);

    Opt.c_cflag &= ~CSTOPB;  // one stop bit
    Opt.c_cflag |= (CLOCAL | CREAD);

    // raw input/output
    cfmakeraw(&Opt);  // no parity, Mask the character size bits, 8 bit
    Opt.c_cflag |= PARENB;
    Opt.c_cflag &= ~PARODD;
    Opt.c_iflag |= (INPCK /*| ISTRIP*/);
    Opt.c_cc[VMIN] = 1;
    Opt.c_cc[VTIME] = 0;
    tcflush(fd, TCIOFLUSH);
    if (tcsetattr(fd, TCSANOW, &Opt) != 0)
    {
        throw vendingMachineError("serial tcsetattr error");
    }
    return 0;
}

void serialPort::readData(unsigned char* buf, unsigned int bufSize)
{
    unsigned int len = 0;
    int len1;

    while (len < bufSize) {
        len1 = read(fd, (char*)buf + len, bufSize - len);
        if (-1 == len1) {
            throw vendingMachineError("serial read error");
        }
        len += len1;
    }
#ifdef printfLog
	if (isPrintLog) {
		struct timeval tv;
		struct timezone tz;
		struct tm *p;
		static unsigned int logi = 0;
		static unsigned char logBuf[1024];
		stringstream ss;
		string s = "/sdcard/vendingMachineFile/log", s1;
		char *path;
		char fBuf[1024];
        time_t tY;

        gettimeofday(&tv, &tz);
        tY = tv.tv_sec;
        tY -= 86400;
        p = localtime(&tY);
        ss << 1900 + p->tm_year;
        ss >> s1;
        ss.clear();
        s += s1;
        ss << 1 + p->tm_mon;
        ss >> s1;
        ss.clear();
        s += s1;
        ss << p->tm_mday;
        ss >> s1;
        ss.clear();
        s += s1;
        s += ".txt";
        path = (char*)s.c_str();
        remove(path);
        s = "/sdcard/vendingMachineFile/log";
		p = localtime(&tv.tv_sec);
        ss << 1900 + p->tm_year;
        ss >> s1;
        ss.clear();
        s += s1;
        ss << 1 + p->tm_mon;
        ss >> s1;
        ss.clear();
        s += s1;
        ss << p->tm_mday;
        ss >> s1;
        ss.clear();
        s += s1;
        s += ".txt";
        path = (char*)s.c_str();
        if (-1 == (fd_log = open(path, O_RDWR | O_APPEND | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO))) {
            throw vendingMachineError("open log error");
        }
		if (0 == logi) {
			if (buf[0] != 0xF2) {
				return;
			}
			//gettimeofday(&tv, &tz);
			//p = localtime(&tv.tv_sec);
			len = sprintf(fBuf, "%d/%02d/%02d %02d:%02d:%02d:%03ld  ", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,
				p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec / 1000);
				write(fd_log, fBuf, len);
		}
		memcpy(logBuf + logi, buf, bufSize);
		logi += bufSize;
		if (logi >= 2) {
			if (logBuf[1] >= 0xB0) {
				len = 4;
			}
			else {
				len = logBuf[1] + 4;
			}
			if (logi == len) {
				logi = 0;
				for (int i = 0; i < len; i++) {
					len1 = sprintf(fBuf, "%02x", logBuf[i]);
					write(fd_log, fBuf, len1);
				}
				write(fd_log, "\n", 1);
			}
		}
		close(fd_log);
	}
#endif
}

void serialPort::writeData(unsigned char const* buf, unsigned int bufSize)
{
	unsigned int len = 0;
	int len1;

	while (len < bufSize) {
		len1 = write(fd, (char*)buf + len, bufSize - len);
		if (-1 == len1) {
			throw vendingMachineError("serial write error");
		}
		len += len1;
	}
#ifdef printfLog
	if (isPrintLog) {
		struct timeval tv;
		struct timezone tz;
		struct tm *p;
		stringstream ss;
		string s = "/sdcard/vendingMachineFile/log", s1;
		char *path;
		char fBuf[1024];

        gettimeofday(&tv, &tz);
		p = localtime(&tv.tv_sec);
        ss << 1900 + p->tm_year;
        ss >> s1;
        ss.clear();
        s += s1;
        ss << 1 + p->tm_mon;
        ss >> s1;
        ss.clear();
        s += s1;
        ss << p->tm_mday;
        ss >> s1;
        ss.clear();
        s += s1;
        s += ".txt";
        path = (char*)s.c_str();
        if (-1 == (fd_log = open(path, O_RDWR | O_APPEND | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO))) {
            throw vendingMachineError("open log error");
        }

		//gettimeofday(&tv, &tz);
		//p = localtime(&tv.tv_sec);
		len = sprintf(fBuf, "%d/%02d/%02d %02d:%02d:%02d:%03ld  ", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,
			p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec / 1000);
		write(fd_log, fBuf, len);
		for (int i = 0; i < bufSize; i++) {
			len = sprintf(fBuf, "%02x", buf[i]);
			write(fd_log, fBuf, len);
		}
		write(fd_log, "\n", 1);
		close(fd_log);
	}
#endif
}
