//
// Created by ruwang on 2016/12/6.
//

#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include "protocol.h"
#include "xcept.h"

protocol vendingMachineProtocol(false);
//protocol vendingMachineProtocol;

/*********************************************************************
* @fn      protocol
*
* @brief   构造函数
*          初始化系统变量，读取配置文件，初始化串口
*
* @param   none
*
* @return  none
*/
protocol::protocol()
{
    int fd;
    char c;
    char buf[100];
    unsigned int i;
    char portName[100];
    unsigned int baudRate = 0;

    index = DATA_START_ADDRESS;
    currentIndex = index;
    first = 0;
    last = 0;
    sem_init(&init_sem_id, 0, 0);
    sem_init(&cyclicQueue_sem_id, 0, 1);
    sem_init(&event_sem_id, 0, 0);
    READER_VER[0] = 0x00;
    READER_VER[1] = 0x01;
    isTransactionProcessing = false;
    isAfterSettlementData = false;
    eventBit = 0;
    isVendingMachineAlive = false;

    memcpy(portName, "/dev/", 5);
//    if (-1 == (fd = open("/sdcard/vendingMachineFile/config.ini"/*"config.ini"*/, O_RDONLY))) {  // 读取配置文件，获取串口号，波特率
//        throw vendingMachineError("open config error");
//    }
     open("/sdcard/vendingMachineFile/config.ini"/*"config.ini"*/, O_RDONLY);

    read(fd, &c, 1);
    while (c != 10) {
        i = 0;
        while (c != '=') {
            buf[i++] = c;
            read(fd, &c, 1);
        }
        i = 0;
        if (!memcmp("commport", buf, 8)) {
            read(fd, &c, 1);
            while (c != 10) {
                portName[5 + i] = c;
                i++;
                read(fd, &c, 1);
            }
            portName[5 + i] = 0;  // 串口号
        }
        else if (!memcmp("baudrate", buf, 8)) {
            read(fd, &c, 1);
            while (c != 10) {
                baudRate = baudRate * 10 + c - 48;  // 波特率
                read(fd, &c, 1);
            }
        }
        else if (!memcmp("debug", buf, 5)) {
            read(fd, &c, 1);
            i = 0;
            while (c != 10) {
                buf[i++] = c;
                read(fd, &c, 1);
            }
            if (!memcmp("false", buf, 5)) {
                isPrintLog = false;
            }
            else {
                isPrintLog = true;
            }
        }
        read(fd, &c, 1);
    }
    if (-1 == close(fd)) {
        throw vendingMachineError("close config error");
    }
    serialInitialization(portName/*"/dev/ttymxc1"*/, baudRate/*19200*/);  // 初始化串口
}

/*********************************************************************
* @fn      ~protocol
*
* @brief   析构函数
*          删除信号量，删除共享内存
*
* @param   none
*
* @return  none
*/
protocol::~protocol()
{
}

void protocol::initialization()
{
    sem_wait(&init_sem_id);
}

void* threadProtocol(void* arg)
{

	vendingMachineProtocol.resolveData();
	return NULL;
}

void* threadDaemon(void* arg)
{

	while (true) {
	    vendingMachineProtocol.isVendingMachineAlive = false;
	    sleep(ALIVE_OVERTIME);
	    if (!vendingMachineProtocol.isVendingMachineAlive) {
	        vendingMachineProtocol.setEvent(SERIAL_ERROR_EVENT_ID);
	    }
	}
	return NULL;
}

void protocol::startProtocol()
{
    pthread_t pth_protocol, pth_daemon;
    
    if (pthread_create(&pth_protocol, NULL, threadProtocol, NULL) != 0) {
        throw vendingMachineError("pthread_create protocol error");
    }  
    if (pthread_create(&pth_daemon, NULL, threadDaemon, NULL) != 0) {
        throw vendingMachineError("pthread_create daemon error");
    }  
    initialization();
}

int protocol::startProtocol(char* portNameIn, unsigned int baudRateIn, bool isDebug)
{
    int fd;
    char c;
    char buf[100];
    unsigned int i;
    char portName[100];
    unsigned int baudRate = 0;
    pthread_t pth_protocol, pth_daemon;

    index = DATA_START_ADDRESS;
    currentIndex = index;
    first = 0;
    last = 0;
    sem_init(&init_sem_id, 0, 0);
    sem_init(&cyclicQueue_sem_id, 0, 1);
    sem_init(&event_sem_id, 0, 0);
    READER_VER[0] = 0x00;
    READER_VER[1] = 0x01;
    isTransactionProcessing = false;
    isAfterSettlementData = false;
    eventBit = 0;
    isVendingMachineAlive = false;
    int errNo;

    memcpy(portName, "/dev/", 5);
    if (-1 == (fd = open("/sdcard/vendingMachineFile/config1.ini"/*"config.ini"*/, O_RDONLY))) {  // 读取配置文件，获取串口号，波特率
        isPrintLog = isDebug;
        if ((errNo = serialInitializationReturnError(/*portNameIn*/"/dev/ttymxc1", /*baudRateIn*/19200)) != 0) {
            return errNo;
        }
    }
    else {
        read(fd, &c, 1);
        while (c != 10) {
            i = 0;
            while (c != '=') {
                buf[i++] = c;
                read(fd, &c, 1);
            }
            i = 0;
            if (!memcmp("commport", buf, 8)) {
                read(fd, &c, 1);
                while (c != 10) {
                    portName[5 + i] = c;
                    i++;
                    read(fd, &c, 1);
                }
                portName[5 + i] = 0;  // 串口号
            }
            else if (!memcmp("baudrate", buf, 8)) {
                read(fd, &c, 1);
                while (c != 10) {
                    baudRate = baudRate * 10 + c - 48;  // 波特率
                    read(fd, &c, 1);
                }
            }
            else if (!memcmp("debug", buf, 5)) {
                read(fd, &c, 1);
                i = 0;
                while (c != 10) {
                    buf[i++] = c;
                    read(fd, &c, 1);
                }
                if (!memcmp("false", buf, 5)) {
                    isPrintLog = false;
                }
                else {
                    isPrintLog = true;
                }
            }
            read(fd, &c, 1);
        }
        if (-1 == close(fd)) {
            throw vendingMachineError("close config error");
        }
        if ((errNo = serialInitializationReturnError(portName/*"/dev/ttymxc1"*/, baudRate/*19200*/)) != 0) {
            return errNo;
        }  // 初始化串口
    }
    if (pthread_create(&pth_protocol, NULL, threadProtocol, NULL) != 0) {
        throw vendingMachineError("pthread_create protocol error");
    }
    if (pthread_create(&pth_daemon, NULL, threadDaemon, NULL) != 0) {
        throw vendingMachineError("pthread_create daemon error");
    }
    initialization();
    return 0;
}

/*********************************************************************
* @fn      resolveData
*
* @brief   解析通信协议
*
* @param   none
*
* @return  none
*/
void protocol::resolveData()
{
    timeval starttime, endtime;
    double timeuse;
    bool isReceivedOutputInstruction = false;  // 是否需要接收数据
    unsigned char state = STATE_START;  // 售货机的初始状态
    timeval endtime_transaction;  // 交易计时

    while (1) {
        readData(frame_receive + START_ADDRESS, 1);  // 读串口
        if (START_VALUE == frame_receive[START_ADDRESS]) {  // 是否是起始字节
            readData(frame_receive + INSTRUCTION_ADDRESS, 1);
            switch (frame_receive[INSTRUCTION_ADDRESS]) {  // 解析指令或数据
                case STANDBY_INSTRUCTION :  // 收到standby指令
                    if (checkInstructions()) {  // 检查指令格式是否正确
                        isReceivedOutputInstruction = false;  // 清收到输出指令标志
                        first = last;  // 清空循环队列
                        isTransactionProcessing = false;  // 清交易正在处理标志
                        isAfterSettlementData = false;  // 清是否是结算数据后标志
                        eventBit = 0;
                        isVendingMachineAlive = false;
                        state = STATE_STANDBY;  // 售货机状态设为standby
                        returnAck(ACK1);
                    }
                    break;
                case LIST_REQUEST_INSTRUCTION :  // 收到一览请求指令，全数据发送
                    if (checkInstructions()) {
                        if (STATE_START == state) {
                            returnAck(ACK4);  // 如果售货机状态不为standby，返回ACK4
                        }
                        else {
                            returnAck(ACK1);
                            addReaderControllerFixedData();  // 添加固定数据
                            addReaderControllerStateData(false, false, false);  // 添加状态数据
                            addReaderControllerExceptionData(false, false, false, false);  // 添加异常数据
                            generateData();  // 生成数据包，包括添加起始，结束，长度，校验字段
                            writeData(frame_buf, frame_buf_len);  // 发送数据
                        }
                    }
                    break;
                case INPUT_REQUEST_INSTRUCTION :  // 收到输入请求指令，可发送数据
                    if (checkInstructions()) {
                        isVendingMachineAlive = true;
                        if (STATE_START == state) {
                            returnAck(ACK4);
                        }
                        else {
                            if (isTransactionProcessing) {  // 需判断交易是否超时
                                gettimeofday(&endtime_transaction, 0);
                                timeuse = 1000000 * (endtime_transaction.tv_sec - starttime_transaction.tv_sec) +
                                            endtime_transaction.tv_usec - starttime_transaction.tv_usec;
                                timeuse /= 1000000;
                                if (timeuse > waitingTimeForTransaction) {
                                    //cancelTransaction();  // 超时取消交易
                                }
                            }
                            frame_len_send = 0;
                            usleep(DATA_SEND_T1 * 1000);
                            if (!isCyclicQueueEmpty()) {  // 是否有数据需要发送
                                returnAck(ACK3);
                                popCyclicQueue();  // 弹出数据
                                usleep(DATA_SEND_T1 * 1000);
                                writeData(frame_send, frame_len_send);
                            }
                            else {
                                returnAck(ACK1);
                            }
                            gettimeofday(&starttime, 0);  // 再次输入请求指令计时
                        }
                    }
                    break;
                case OUTPUT_INSTRUCTION :  // 收到输出指令，主控有数据需要发送
                    if (checkInstructions()) {
                        if (STATE_START == state) {
                            returnAck(ACK4);
                        }
                        else {
                            usleep(DATA_RECEIVE_T1 * 1000);  // 延时
                            returnAck(ACK1);
                            isReceivedOutputInstruction = true;  // 准备接收数据
                            gettimeofday(&starttime, 0);  // 再次输出指令计时
                        }
                    }
                    break;
                case INPUT_REQUEST_MORE_INSTRUCTION :  // 收到再次输入请求指令，之前发的ack或数据可能丢掉，需再次发送
                    if (checkInstructions()) {
                        if (STATE_START == state) {
                            returnAck(ACK4);
                        }
                        else {
                            gettimeofday(&endtime, 0);
                            timeuse = 1000000 * (endtime.tv_sec - starttime.tv_sec) + endtime.tv_usec - starttime.tv_usec;
                            timeuse /= 1000;
                            usleep(DATA_SEND_T1 * 1000);
                            if (timeuse < INPUT_MORE_REQUEST_OVERTIME) {  // 从输入请求指令到再次输入请求指令的时间需小于INPUT_MORE_REQUEST_OVERTIME
                                if (frame_len_send > 0) {  // 有数据需要发送
                                    returnAck(ACK3);
                                    usleep(DATA_SEND_T1 * 1000);
                                    writeData(frame_send, frame_len_send);
                                }
                                else {
                                    returnAck(ACK1);
                                }
                            }
                            else {
                                returnAck(NAK);  // 超时
                            }
                            gettimeofday(&starttime, 0);
                        }
                    }
                    break;
                case OUTPUT_MORE_INSTRUCTION :  // 收到再次输出指令，ack或数据可能丢掉，需重新接收
                    if (checkInstructions()) {
                        if (STATE_START == state) {
                            returnAck(ACK4);
                        }
                        else {
                            gettimeofday(&endtime, 0);
                            timeuse = 1000000 * (endtime.tv_sec - starttime.tv_sec) + endtime.tv_usec - starttime.tv_usec;
                            timeuse /= 1000;
                            usleep(DATA_RECEIVE_T1 * 1000);
                            if (timeuse < OUTPUT_MORE_INSTRUCTION_OVERTIME) {  // 从输出指令到再次输出指令的时间需小于OUTPUT_MORE_INSTRUCTION_OVERTIME
                                if (!isReceivedOutputInstruction) {
                                    returnAck(ACK5);  // 数据已收到
                                }
                                else {
                                    returnAck(ACK1);
                                }
                            }
                            else {
                                returnAck(NAK);
                            }
                            gettimeofday(&starttime, 0);
                        }
                    }
                    break;
                case NAK1_INSTRUCTION :
                    returnAck(NAK);
                    break;
                case NAK2_INSTRUCTION :
                    returnAck(NAK);
                    break;
                default :
                    if (isReceivedOutputInstruction) {  // 接收数据
                        if (checkData()) {  // 检查数据格式，校验
                            usleep(DATA_RECEIVE_T1 * 1000);
                            if (resolveDC()) {  // 解析数据DC
                                returnAck(ACK1);
                                isReceivedOutputInstruction = false;  // 数据已收到，清标志
                            }
                            else {
                                returnAck(NAK);  // 不能识别的数据DC，返回NAK
                            }
                            gettimeofday(&starttime, 0);
                        }
                    }
                    break;
            }
        }
    }
}

/*********************************************************************
* @fn      checkInstructions
*
* @brief   检查指令格式
*
* @param   none
*
* @return  true：指令格式正确，false：指令格式错误
*/
bool const protocol::checkInstructions()
{
    bool b = false;

    readData(frame_receive + NOT_INSTRUCTION_ADDRESS, 1);
    if (frame_receive[INSTRUCTION_ADDRESS] == (unsigned char)~frame_receive[NOT_INSTRUCTION_ADDRESS]) {
        readData(frame_receive + END_ADDRESS, 1);
        if (END_VALUE == frame_receive[END_ADDRESS]) {
            b = true;
        }
    }
    return b;
}

/*********************************************************************
* @fn      returnAck
*
* @brief   返回ACK
*
* @param   输入参数 ack - ack码
*
* @return  none
*/
void protocol::returnAck(unsigned char ack)
{
    writeData(&ack, 1);
}

/*********************************************************************
* @fn      addDc
*
* @brief   添加数据DC
*
* @param   输入参数 dc - 数据DC
*
* @return  none
*/
void protocol::addDc(unsigned char dc)
{
    frame_buf[currentIndex] = index - currentIndex - 1;  // 设置上一个DC的长度
    currentIndex = index;
    frame_buf[++index] = dc;
    index++;
}

/*********************************************************************
* @fn      addData
*
* @brief   添加数据
*
* @param   输入参数 buf - 数据指针
* @param   输入参数 size - 数据长度
*
* @return  none
*/
void protocol::addData(unsigned char const* buf, unsigned int size)
{
    memcpy(frame_buf + index, buf, size);
    index += size;
}

/*********************************************************************
* @fn      generateData
*
* @brief   生成数据包，包括添加起始，结束，长度，校验字段
*
* @param   none
*
* @return  none
*/
void protocol::generateData()
{
    frame_buf[currentIndex] = index - currentIndex - 1;
    frame_buf[TOTAL_LEN_ADDRESS] = index - DATA_START_ADDRESS;  // 设置总长度
    frame_buf[START_ADDRESS] = START_VALUE;
    setFCC();
    frame_buf[index++] = END_VALUE;
    frame_buf_len = index;  // 数据包的长度
    index = DATA_START_ADDRESS;
    currentIndex = index;
}

/*********************************************************************
* @fn      setFCC
*
* @brief   设置校验字段
*
* @param   none
*
* @return  none
*/
void protocol::setFCC()
{
    unsigned int i;
    unsigned char c = 0;

    for (i = TOTAL_LEN_ADDRESS; i < index; i++) {
        c ^= frame_buf[i];
    }
    frame_buf[index++] = c;
}

/*********************************************************************
* @fn      addReaderControllerFixedData
*
* @brief   添加固定数据 DC = 0x4D
*
* @param   none
*
* @return  none
*/
void protocol::addReaderControllerFixedData()
{
    addDc(READER_CONTROLLER_FIXED_DATA_DC);
    addData(READER_CONTROLLER_COUNTRY_CODE, sizeof(READER_CONTROLLER_COUNTRY_CODE));
    addData(READER_CONTROLLER_CLIENT_CODE, sizeof(READER_CONTROLLER_CLIENT_CODE));
    addData(READER_CONTROLLER_VER, sizeof(READER_CONTROLLER_VER));
    addData(READER_VER, sizeof(READER_VER));
}

/*********************************************************************
* @fn      addReaderControllerStateData
*
* @brief   添加状态数据 DC = 0x5B
*
* @param   输入参数 isCardAccepted - 卡是否可受理
* @param   输入参数 isTransactionCancelled - 交易是否取消
* @param   输入参数 isInProcessing - 交易是否正在处理中
*
* @return  none
*/
void protocol::addReaderControllerStateData(bool isCardAccepted, bool isTransactionCancelled, bool isInProcessing)
{
    unsigned char c = 0;

    addDc(READER_CONTROLLER_STATE_DATA_DC);
    if (!isCardAccepted) {
        c |= 0x01;
    }
    if (isTransactionCancelled) {
        c |= 0x02;
    }
    if (isInProcessing) {
        c |= 0x04;
    }
    addData(&c, sizeof(c));
}

/*********************************************************************
* @fn      addReaderControllerExceptionData
*
* @brief   添加异常数据 DC = 0x5A
*
* @param   输入参数 isReaderException - 是否读卡器端有异常
* @param   输入参数 isReaderNoResponse - 是否读卡器无应答
* @param   输入参数 isReaderCommunicationException - 是否读卡器通信异常
* @param   输入参数 isReaderInitializationException - 是否读卡器初始化异常
*
* @return  none
*/
void protocol::addReaderControllerExceptionData(bool isReaderException, bool isReaderNoResponse,
                                                bool isReaderCommunicationException, bool isReaderInitializationException)
{
    unsigned char c = 0;

    addDc(READER_CONTROLLER_EXCEPTION_DATA_DC);
    if (isReaderException) {
        c |= 0x01;
    }
    if (isReaderNoResponse) {
        c |= 0x02;
    }
    if (isReaderCommunicationException) {
        c |= 0x04;
    }
    if (isReaderInitializationException) {
        c |= 0x08;
    }
    addData(&c, sizeof(c));
}

/*********************************************************************
* @fn      checkData
*
* @brief   检查数据格式和校验是否正确
*
* @param   none
*
* @return  true：格式和校验正确，false：格式或校验错误
*/
bool const protocol::checkData()
{
    unsigned char i;
    unsigned char c = 0;
    bool b = false;

    readData(frame_receive + DATA_START_ADDRESS, frame_receive[TOTAL_LEN_ADDRESS]);
    for (i = 0; i < frame_receive[TOTAL_LEN_ADDRESS] + 1; i++) {
        c ^= frame_receive[i + TOTAL_LEN_ADDRESS];
    }
    readData(frame_receive + DATA_START_ADDRESS + frame_receive[TOTAL_LEN_ADDRESS], FCC_LEN);
    if (frame_receive[frame_receive[TOTAL_LEN_ADDRESS] + TOTAL_LEN_LEN + START_LEN] == c) {
        readData(frame_receive + DATA_START_ADDRESS + frame_receive[TOTAL_LEN_ADDRESS] + FCC_LEN, END_LEN);
        if (END_VALUE == frame_receive[frame_receive[TOTAL_LEN_ADDRESS] + TOTAL_LEN_LEN + START_LEN + FCC_LEN]) {
            b = true;
        }
    }
    return b;
}

/*********************************************************************
* @fn      resolveDC
*
* @brief   解析，存储，更新售货机返回的数据，包括交易数据，控制数据，货道状况，故障信息，单价，温度，状态，指令执行结果
*
* @param   none
*
* @return  true ： 解析成功，false ： 有无法识别的DC
*/
bool const protocol::resolveDC()
{
    unsigned int len = 0;
    bool b = true;
    unsigned int i;

    while (len < frame_receive[TOTAL_LEN_ADDRESS]) {
        switch (frame_receive[DATA_START_ADDRESS + len + 1]) {
            case ENVIRONMENTAL_DATA_DC :  // 存储环境数据
                memcpy(vendingMachineNumber, frame_receive + DATA_START_ADDRESS + len + 2, 4);
                memcpy(vendingMachineDate, frame_receive + DATA_START_ADDRESS + len + 6, 8);
                waitingTimeForTransaction = WAITING_TIME_FOR_CONTACT/*frame_receive[DATA_START_ADDRESS + len + 14]*/;
                break;
            case CONTROL_DATA_DC :  // 收到控制数据
                if ((frame_receive[DATA_START_ADDRESS + len + 2] & 0x80) != 0x80) {
                    sem_wait(&cyclicQueue_sem_id);
                    if (0x01 == (frame_receive[DATA_START_ADDRESS + len + 2] & 0x01)) {  // 卡允许
                        addReaderControllerStateData(true, false, false);
                        setEvent(ALLOWS_ONLINE_TRANSACTION_EVENT_ID);
                    }
                    else {  // 卡禁止
                        if (isAfterSettlementData) {  // 跟在结算数据后回0x5B01
                            addReaderControllerStateData(false, false, false);
                            isAfterSettlementData = false;
                        }
                        else {  // 其他情况回0x5B03
                            addReaderControllerStateData(false, true, false);
                        }
                        setEvent(BAN_ONLINE_TRADING_EVENT_ID);  // 设置投现金的事件
                    }
                    generateData();
                    pushCyclicQueue();
                    sem_post(&cyclicQueue_sem_id);
                }
                else {  // 交易结束
                    isTransactionProcessing = false;
                    setEvent(TRANSACTION_END_EVENT_ID);
                }
                break;
            case SELECTING_DATA_DC :  // 收到货物选择数据，存储货道号和金额
                currentColumnNo = frame_receive[DATA_START_ADDRESS + len + 2];
                currentAmount = frame_receive[DATA_START_ADDRESS + len + 3] * 256 + frame_receive[DATA_START_ADDRESS + len + 4];
                popCyclicQueue();  // 不回0x5B
                isTransactionProcessing = true;  // 设置交易正在处理标志
                setEvent(PUSH_BUTTON_EVENT_ID);  // 设置按键事件
                gettimeofday(&starttime_transaction, 0);  // 收到选货信息，交易计时
                break;
            case TOTAL_SALES_DC :  // 存储总销售额的各项数据
                memcpy(totalSalesCount, frame_receive + DATA_START_ADDRESS + len + 2, 4);
                memcpy(totalSalesAmount, frame_receive + DATA_START_ADDRESS + len + 6, 4);
                memcpy(totalSalesCountByCash, frame_receive + DATA_START_ADDRESS + len + 10, 4);
                memcpy(totalSalesAmountByCash, frame_receive + DATA_START_ADDRESS + len + 14, 4);
                memcpy(totalSalesCountByNotCash, frame_receive + DATA_START_ADDRESS + len + 18, 4);
                memcpy(totalSalesAmountByNotCash, frame_receive + DATA_START_ADDRESS + len + 22, 4);
                break;
            case NUMBER_SALES_CHANNELS_DC :  // 存储各货道的销售数量
                columnCount = (frame_receive[DATA_START_ADDRESS + len] - 1) / 4;
                for (i = 0; i < columnCount; i++) {
                    memcpy(saleCountsPerColumn[i], frame_receive + DATA_START_ADDRESS + len + 2 + i * 4, 4);
                }
                break;
            case SALES_CHANNELS_DC :  // 存储各货道销售金额
                for (i = 0; i < columnCount; i++) {
                    memcpy(saleAmountPerColumn[i], frame_receive + DATA_START_ADDRESS + len + 2 + i * 4, 4);
                }
                break;
            case INDIVIDUAL_SALES_INFORMATION_DC :  // 表明交易成功，更新销售货道的数量和金额
                currentColumnNo = frame_receive[DATA_START_ADDRESS + len + 2];
                memcpy(saleCountsPerColumn[currentColumnNo - 1], frame_receive + DATA_START_ADDRESS + len + 3, 4);
                memcpy(saleAmountPerColumn[currentColumnNo - 1], frame_receive + DATA_START_ADDRESS + len + 7, 4);
                setEvent(OUT_GOODS_SUCCESS_EVENT_ID);  // 设置出货成功事件
                break;
            case EMPTY_CHANNELS_DC :  // 存储各个货道的售空信息
                for (i = 0; i < (columnCount + 7) / 8; i++) {
                    saleSoldoutPerColumn[i] = frame_receive[DATA_START_ADDRESS + len + 2 + i];
                }
                setEvent(GOODS_SOLD_OUT_STATE_EVENT_ID);
                break;
            case FAULT_MESSAGE_DC :  // 存储故障信息
                faultInformationCount = (frame_receive[DATA_START_ADDRESS + len] - 1) / 2;
                for (i = 0; i < faultInformationCount; i++) {
                    memcpy(faultInformation[i], frame_receive + DATA_START_ADDRESS + len + 2 + i * 2, 2);
                }
                setEvent(FAULT_MESSAGE_EVENT_ID);
                break;
            case BEVERAGE_INFORMATION_CHANNELS_DC :  // 存储各个货道的饮料代码信息
                for (i = 0; i < columnCount; i++) {
                    memcpy(beverageInformationPerColumn[i], frame_receive + DATA_START_ADDRESS + len + 2 + i * 3, 3);
                }
                break;
            case BEVERAGE_PRICE_CHANNELS_DC :  // 存储各个货道的饮料单价
                for (i = 0; i < columnCount; i++) {
                    memcpy(beveragePricePerColumn[i], frame_receive + DATA_START_ADDRESS + len + 2 + i * 2, 2);
                }
                break;
            case TEMPERATURE_SETTING_CHANNELS_DC :  // 存储各个货道的温度设定
                for (i = 0; i < columnCount; i++) {
                    temperaturePerColumn[i] = frame_receive[DATA_START_ADDRESS + len + 2 + i];
                }
                break;
            case STATE_DATA_DC :  // 存储售货机的状态数据
                stateData = frame_receive[DATA_START_ADDRESS + len + 2];
                setEvent(STATE_CHANGE_EVENT_ID);
                break;
            case LABEL_INFORMATION_DC :  // 存储售货机的标号信息
                memcpy(vendingMachineNumber, frame_receive + DATA_START_ADDRESS + len + 2, 4);
                columnCount = frame_receive[DATA_START_ADDRESS + len + 6];
                memcpy(vmcFirmwareVersion, frame_receive + DATA_START_ADDRESS + len + 7, 3);
                break;
            case SHIPPING_RESULTS_DC :  // 保存出货结果
                if (0 == frame_receive[DATA_START_ADDRESS + len + 2]) {
                    setEvent(OUT_GOODS_FAIL_EVENT_ID);
                }
                currentColumnNo = frame_receive[DATA_START_ADDRESS + len + 3];
                break;
            case RESPONSE_INFORMATION_DC :  // 保存价格设定结果
                processingInstruction = frame_receive[DATA_START_ADDRESS + len + 2];
                processedResult = frame_receive[DATA_START_ADDRESS + len + 3];
                setEvent(SET_PRICE_RESULT_EVENT_ID);
                break;
            case BEVERAGE_CARD_PRICE_CHANNELS_DC :  // 存储各个货道饮料的卡单价
                for (i = 0; i < columnCount; i++) {
                    memcpy(beverageCardPricePerColumn[i], frame_receive + DATA_START_ADDRESS + len + 2 + i * 2, 2);
                }
                break;
            case SETTING_INFORMATION_DC :  // 保存售货机的设定信息
                vmcSettingData = frame_receive[DATA_START_ADDRESS + len + 3];
                sem_post(&init_sem_id);  // 与售货机的初次交互完成
                break;
            default :
                b = false;
                break;
        }
        len += frame_receive[DATA_START_ADDRESS + len] + 1;
    }
    return b;
}

void protocol::getGoodsSoldOutState(unsigned char* goodsSoldOutState)
{
    memcpy(goodsSoldOutState, saleSoldoutPerColumn, (columnCount + 7) / 8);
}

/*********************************************************************
* @fn      cancelTransaction
*
* @brief   取消交易
*
* @param   none
*
* @return  none
*/
void protocol::cancelTransaction()
{
    sem_wait(&cyclicQueue_sem_id);
    addReaderControllerStateData(true, true, false);
    generateData();
    pushCyclicQueue();
    sem_post(&cyclicQueue_sem_id);
}

/*********************************************************************
* @fn      processingPayment
*
* @brief   开始结算
*
* @param   none
*
* @return  none
*/
void protocol::processingPayment()
{
    sem_wait(&cyclicQueue_sem_id);
    addReaderControllerStateData(true, false, true);
    generateData();
    pushCyclicQueue();
    sem_post(&cyclicQueue_sem_id);
    gettimeofday(&starttime_transaction, 0);  // 交易计时
}

/*********************************************************************
* @fn      addSettlementData
*
* @brief   添加结算数据
*
* @param   none
*
* @return  none
*/
void protocol::addSettlementData()
{
    unsigned char buf[11];

    addDc(SETTLEMENT_DATA_DC);
    buf[0] = 0x41;
    buf[1] = currentColumnNo;
    buf[2] = 0x41;
    buf[3] = currentAmount >> 8;
    buf[4] = currentAmount;
    memset(buf + 5, 0, 6);
    toBCD(0x0D9038, buf + 7);
    toBCD(0x0D9038, buf + 10);
    addData(buf, 11);
}

/*********************************************************************
* @fn      outGoods
*
* @brief   按键非B0出货
*
* @param   none
*
* @return  none
*/
void protocol::outGoods()
{
    processingPayment();  // 交易正在处理

    sem_wait(&cyclicQueue_sem_id);
    addReaderControllerStateData(false, false, false);  // 卡禁止
    generateData();
    pushCyclicQueue();

    addSettlementData();  // 添加结算数据
    generateData();
    pushCyclicQueue();
    sem_post(&cyclicQueue_sem_id);
    isAfterSettlementData = true;
}

/*********************************************************************
* @fn      toBCD
*
* @brief   生成BCD码
*
* @param   输入参数 amount - 金额
* @param   输入参数 buf - BCD码存储地址
*
* @return  none
*/
void protocol::toBCD(unsigned int amount, unsigned char* buf) const
{
    unsigned int remainder = amount % 10;
    unsigned int i = 0;

    while (amount > 0 || remainder > 0) {
        if (0 == i) {
            *buf = remainder;
            i++;
        }
        else {
            *buf |= (remainder << 4);
            buf--;
            i = 0;
        }
        amount = amount / 10;
        remainder = amount % 10;
    }
}

/*********************************************************************
* @fn      pushSharedMemoryBuf
*
* @brief   数据写入循环队列，数据会在INPUT_REQUEST_INSTRUCTION指令里被发送
*
* @param   none
*
* @return  none
*/
void protocol::pushCyclicQueue()
{
    unsigned int i;

    if (getCyclicQueueLength() + frame_buf_len > BUF_SZ - 1) {
        throw vendingMachineError("cyclic queue full");
    }
    for (i = 0; i < frame_buf_len; i++) {
        cyclicQueue[(last + i) % BUF_SZ] = frame_buf[i];
    }
    last = (last + frame_buf_len) % BUF_SZ;
}

/*********************************************************************
* @fn      popSharedMemoryBuf
*
* @brief   从循环队列里弹出数据
*
* @param   none
*
* @return  none
*/
void protocol::popCyclicQueue()
{
    unsigned int i;

    sem_wait(&cyclicQueue_sem_id);
    frame_send[0] = cyclicQueue[first];
    frame_send[1] = cyclicQueue[(first + 1) % BUF_SZ];
    for (i = 0; i < frame_send[1]; i++) {
        frame_send[i + 2] = cyclicQueue[(first + 2 + i) % BUF_SZ];
    }
    frame_send[frame_send[1] + 2] = cyclicQueue[(first + 2 + frame_send[1]) % BUF_SZ];
    frame_send[frame_send[1] + 2 + 1] = cyclicQueue[(first + 2 + frame_send[1] + 1) % BUF_SZ];
    first = (first + 2 + frame_send[1] + 2) % BUF_SZ;
    frame_len_send = frame_send[1] + 4;
    sem_post(&cyclicQueue_sem_id);
}

/*********************************************************************
* @fn      outGoodsUseB0
*
* @brief   B0直接出货
*
* @param   输入参数 columnNo - 货道号
* @param   输入参数 salesCategory - 销售种类
*
* @return  none
*/
void protocol::outGoodsUseB0(unsigned char columnNo, unsigned char salesCategory)
{
    isTransactionProcessing = true;
    gettimeofday(&starttime_transaction, 0);
    sem_wait(&cyclicQueue_sem_id);
    addDc(SHIPPING_INSTRUCTION_DC);
    addData(&columnNo, 1);
    addData(&salesCategory, 1);
    generateData();
    pushCyclicQueue();
    sem_post(&cyclicQueue_sem_id);
}

/*********************************************************************
* @fn      outGoodsUseB0ByButton
*
* @brief   按键B0出货
*
* @param   输入参数 salesCategory - 销售种类
*
* @return  none
*/
void protocol::outGoodsUseB0ByButton(unsigned char salesCategory)
{
    sem_wait(&cyclicQueue_sem_id);
    //addReaderControllerStateData(true, true, false);  // 先取消交易
    //generateData();
    //pushCyclicQueue();

    addDc(SHIPPING_INSTRUCTION_DC);  // B0出货
    addData(&currentColumnNo, 1);
    addData(&salesCategory, 1);
    generateData();
    pushCyclicQueue();
    sem_post(&cyclicQueue_sem_id);
}

/*********************************************************************
* @fn      getSelectedGoodInformation
*
* @brief   获取按键选择商品的金额和货道号
*
* @param   输出参数 amount - 返回金额
* @param   输出参数 columnNo - 返回货道号
*
* @return  none
*/
void protocol::getSelectedGoodInformation(unsigned short& amount, unsigned char& columnNo)
{
    amount = currentAmount;
    columnNo = currentColumnNo;
}

unsigned short protocol::getSelectedGoodAmount()
{
    return currentAmount;
}

unsigned char protocol::getSelectedGoodColumnNo()
{
    return currentColumnNo;
}

/*********************************************************************
* @fn      getSetPriceResults
*
* @brief   获取单价设置是否成功
*
* @param   输出参数 instruction - 返回当前设置价格的指令
*
* @return  true：成功，false：失败
*/
bool protocol::getSetPriceResults(unsigned char& instruction)
{
    instruction = processingInstruction;
    if (0 == processedResult) {
        return true;
    }
    else {
        return false;
    }
}

bool protocol::getSetPriceResults()
{
    if (0 == processedResult) {
        return true;
    }
    else {
        return false;
    }
}

/*********************************************************************
* @fn      setColumnPrice
*
* @brief   设置所有货道的单价，必须是所有货道的单价一起设置
*
* @param   输入参数 price - 单价数组，数组长度为 货道数 * 2，每个单价为2个字节，高字节在前，
*                           单价以角为单位，必须是5的倍数
*
* @return  none
*/
void protocol::setColumnPrice(unsigned char const* price)
{
    unsigned int i;

    for (i = 0; i < columnCount; i++) {
        memcpy(beveragePricePerColumn[i], price + i * 2, 2);
    }
    sem_wait(&cyclicQueue_sem_id);
    addDc(BEVERAGE_PRICE_CHANNELS_DC);
    for (i = 0; i < columnCount; i++) {
        addData(beveragePricePerColumn[i], 2);
    }
    generateData();
    pushCyclicQueue();
    sem_post(&cyclicQueue_sem_id);
}

/*********************************************************************
* @fn      getEvent
*
* @brief   获取事件id号
*
* @param   none
*
* @return  事件id号
*/
unsigned int const protocol::getEvent()
{
    unsigned int eventId;
    
    sem_wait(&event_sem_id);  // 等待事件发生
    if (eventBit & PUSH_BUTTON_EVENT_ID) {
        eventBit &= ~PUSH_BUTTON_EVENT_ID;
        eventId = PUSH_BUTTON_EVENT_ID;
    }
    else if (eventBit & OUT_GOODS_SUCCESS_EVENT_ID) {
        eventBit &= ~OUT_GOODS_SUCCESS_EVENT_ID;
        eventId = OUT_GOODS_SUCCESS_EVENT_ID;
    }
    else if (eventBit & BAN_ONLINE_TRADING_EVENT_ID) {
        eventBit &= ~BAN_ONLINE_TRADING_EVENT_ID;
        eventId = BAN_ONLINE_TRADING_EVENT_ID;
    }
    else if (eventBit & ALLOWS_ONLINE_TRANSACTION_EVENT_ID) {
        eventBit &= ~ALLOWS_ONLINE_TRANSACTION_EVENT_ID;
        eventId = ALLOWS_ONLINE_TRANSACTION_EVENT_ID;
    }
    else if (eventBit & OUT_GOODS_FAIL_EVENT_ID) {
        eventBit &= ~OUT_GOODS_FAIL_EVENT_ID;
        eventId = OUT_GOODS_FAIL_EVENT_ID;
    }
    else if (eventBit & SET_PRICE_RESULT_EVENT_ID) {
        eventBit &= ~SET_PRICE_RESULT_EVENT_ID;
        eventId = SET_PRICE_RESULT_EVENT_ID;
    }
    else if (eventBit & TRANSACTION_END_EVENT_ID) {
        eventBit &= ~TRANSACTION_END_EVENT_ID;
        eventId = TRANSACTION_END_EVENT_ID;
    }
    else if (eventBit & STATE_CHANGE_EVENT_ID) {
        eventBit &= ~STATE_CHANGE_EVENT_ID;
        eventId = STATE_CHANGE_EVENT_ID;
    }
    else if (eventBit & SERIAL_ERROR_EVENT_ID) {
        eventBit &= ~SERIAL_ERROR_EVENT_ID;
        eventId = SERIAL_ERROR_EVENT_ID;
    }
    else if (eventBit & GOODS_SOLD_OUT_STATE_EVENT_ID) {
        eventBit &= ~GOODS_SOLD_OUT_STATE_EVENT_ID;
        eventId = GOODS_SOLD_OUT_STATE_EVENT_ID;
    }
    else if (eventBit & FAULT_MESSAGE_EVENT_ID) {
        eventBit &= ~FAULT_MESSAGE_EVENT_ID;
        eventId = FAULT_MESSAGE_EVENT_ID;
    }
    return eventId;
}

/*********************************************************************
* @fn      getCurrentSalesInformation
*
* @brief   交易成功后，获取此次交易的货道号，此货道号的销售数量和销售金额
*
* @param   输出参数 columnNo - 返回货道号
* @param   输入参数 salesCount - 存放销售数量的地址，长度为4个字节
* @param   输入参数 salesAmount - 存放销售金额的地址，长度为4个字节
*
* @return  none
*/
void protocol::getCurrentSalesInformation(unsigned char& columnNo, unsigned char* salesCount, unsigned char* salesAmount)
{
    columnNo = currentColumnNo;
    memcpy(salesCount, saleCountsPerColumn[columnNo - 1], 4);
    memcpy(salesAmount, saleAmountPerColumn[columnNo - 1], 4);
}

unsigned char protocol::getCurrentSalesColumnNo()
{
    return currentColumnNo;
}

void protocol::getCurrentSalesCount(unsigned char* salesCount)
{
    memcpy(salesCount, saleCountsPerColumn[currentColumnNo - 1], 4);
}

void protocol::getCurrentSalesAmount(unsigned char* salesAmount)
{
    memcpy(salesAmount, saleAmountPerColumn[currentColumnNo - 1], 4);
}

/*********************************************************************
* @fn      selectGoodsByPC
*
* @brief   pc端选货
*
* @param   输入参数 columnNo - 货道号
*
* @return  none
*/
void protocol::selectGoodsByScreen(unsigned char columnNo)
{
    sem_wait(&cyclicQueue_sem_id);
    addDc(PC_SELECT_GOODS_DC);
    addData(&columnNo, 1);
    generateData();
    pushCyclicQueue();
    sem_post(&cyclicQueue_sem_id);
}

/*********************************************************************
* @fn      setEvent
*
* @brief   设置事件
*
* @param   输入参数 eventId - 事件id号
*
* @return  none
*/
void protocol::setEvent(unsigned int eventId)
{
    eventBit |= eventId;
    sem_post(&event_sem_id);
}

unsigned char protocol::getStateData()
{
    return stateData;
}

unsigned int protocol::getFaultInformationCount()
{
    return faultInformationCount;
}

void protocol::getFaultInformation(unsigned char* faultBuf)
{
    int i;
    
    for (i = 0; i < faultInformationCount; i++) {
        memcpy(faultBuf + i * 2, faultInformation[i], 2);
    }
}

void protocol::getVendingMachineNumber(unsigned char* vMN)
{
    memcpy(vMN, vendingMachineNumber, 4);
}
