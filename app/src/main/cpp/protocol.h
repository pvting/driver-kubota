//
// Created by ruwang on 2016/12/6.
//

#ifndef MYAPPLICATION_PROTOCOL_H
#define MYAPPLICATION_PROTOCOL_H

#include <semaphore.h>
#include "serialPort.h"

#define INSTRUCTION_LEN 1  // 指令字段的字节长度
#define FCC_LEN 1  // 校验码字段的字节长度
#define END_LEN 1  // 结束符字段的字节长度
#define TOTAL_LEN_LEN 1  // 总长度字段的字节长度
#define START_LEN 1  // 起始符字段的字节长度
#define NOT_INSTRUCTION_LEN 1  // 反转指令字段的字节长度

#define START_ADDRESS 0  // 起始符字段的地址
#define INSTRUCTION_ADDRESS (START_ADDRESS + START_LEN)  // 指令字段的地址
#define NOT_INSTRUCTION_ADDRESS (INSTRUCTION_ADDRESS + INSTRUCTION_LEN)  // 反转指令的地址
#define END_ADDRESS (NOT_INSTRUCTION_ADDRESS + NOT_INSTRUCTION_LEN)  // 结束符字段的地址
#define TOTAL_LEN_ADDRESS (START_ADDRESS + START_LEN)  // 总长度字段的地址
#define DATA_START_ADDRESS (TOTAL_LEN_ADDRESS + TOTAL_LEN_LEN)  // 数据的起始地址

#define START_VALUE 0xF2  // 起始字符
#define END_VALUE 0xFF  // 结束字符

#define STANDBY_INSTRUCTION 0xB0  // 待机指令
#define LIST_REQUEST_INSTRUCTION 0xB1  // 一览请求指令
#define INPUT_REQUEST_INSTRUCTION 0xB2  // 输入请求指令
#define OUTPUT_INSTRUCTION 0xB3  // 输出指令
#define INPUT_REQUEST_MORE_INSTRUCTION 0xB4  // 再次输入请求指令
#define OUTPUT_MORE_INSTRUCTION 0xB5  // 再次输出指令
#define NAK1_INSTRUCTION 0xB6  // 未定义
#define NAK2_INSTRUCTION 0xB7  // 未定义

#define ACK1 0x11  // 其他
#define ACK2 0x22
#define ACK3 0x33  // 有数据需要发送时
#define ACK4 0x44  // 启动后待机指令以外的指令发送时
#define ACK5 0x55  // 数据已接收，回复再次输出指令时
#define NAK 0xEE  // 指令或数据无法受理时

#define READER_CONTROLLER_FIXED_DATA_DC 0x4D  // 固定数据
unsigned char const READER_CONTROLLER_COUNTRY_CODE[2] = {0x11, 0x56};  // 国家代码
unsigned char const READER_CONTROLLER_CLIENT_CODE[2] = {0x00, 0x26};  // 客户代码
unsigned char const READER_CONTROLLER_VER[1] = {0x00};  // 售货机版本号

#define PC_SELECT_GOODS_DC 0x57  // pc端选择数据
#define SETTLEMENT_DATA_DC 0x59  // 结算数据
#define READER_CONTROLLER_STATE_DATA_DC 0x5B  // 状态数据
#define READER_CONTROLLER_EXCEPTION_DATA_DC 0x5A  // 异常数据
#define ENVIRONMENTAL_DATA_DC 0x55  // 环境数据
#define CONTROL_DATA_DC 0x50  // 控制数据
#define SELECTING_DATA_DC 0x51  // 选择数据
#define TOTAL_SALES_DC 0xD0  // 总销售额
#define NUMBER_SALES_CHANNELS_DC 0xD1  // 各货道的销售数量
#define SALES_CHANNELS_DC 0xD2  // 各货道销售金额
#define INDIVIDUAL_SALES_INFORMATION_DC 0xD5  // 销售货道的信息
#define EMPTY_CHANNELS_DC 0xC0  // 各个货道的售空信息
#define FAULT_MESSAGE_DC 0xC1  // 故障信息
#define BEVERAGE_INFORMATION_CHANNELS_DC 0xC2  // 各个货道的饮料代码信息
#define BEVERAGE_PRICE_CHANNELS_DC 0xC3  // 各个货道的饮料单价
#define TEMPERATURE_SETTING_CHANNELS_DC 0xC4  // 各个货道的温度设定
#define STATE_DATA_DC 0xC5  // 售货机的状态数据
#define LABEL_INFORMATION_DC 0xC6  // 售货机的标号信息
#define BEVERAGE_CARD_PRICE_CHANNELS_DC 0xC7  // 各个货道饮料的卡单价
#define SETTING_INFORMATION_DC 0xC8  // 售货机的设定信息
#define SHIPPING_INSTRUCTION_DC 0xB0  // 出货指令
#define SHIPPING_RESULTS_DC 0xE1  // 出货结果
#define RESPONSE_INFORMATION_DC 0xE2  // 指令处理的结果

// 销售种类
#define CASH 0  // 现金
#define BEYOND_CASH 1  // 现金以外
#define OTHER 2  // 其他

#define STATE_START 0  // 启动状态
#define STATE_STANDBY 1  // 待机状态

#define DATA_RECEIVE_T1 1// 20  // millisecond 数据接收延时
#define DATA_SEND_T1 1
#define OUTPUT_MORE_INSTRUCTION_OVERTIME 2000  // millisecond 再次输出指令超时
#define INPUT_MORE_REQUEST_OVERTIME 2000  // millisecond 再次输入请求指令超时

#define BUF_SZ 1024  // 共享内存里循环队列的size

// 结算失败
#define BALANCE_LOW 1  // 余额不足
#define OTHER_PAYMENT_ERROR 0  // 其他原因

#define MAX_COLUMN_COUNT 200  // 货道数的最大值

#define WAITING_TIME_FOR_CONTACT 120  // second 交易超时时间
#define ALIVE_OVERTIME 10

#define PUSH_BUTTON_EVENT_ID 0x01  // 按键事件
#define OUT_GOODS_SUCCESS_EVENT_ID 0x02  // 出货成功事件
#define BAN_ONLINE_TRADING_EVENT_ID 0x04  // 线上交易禁止事件
#define ALLOWS_ONLINE_TRANSACTION_EVENT_ID 0x08  // 线上交易允许事件
#define OUT_GOODS_FAIL_EVENT_ID 0x10  // 出货失败事件
#define SET_PRICE_RESULT_EVENT_ID 0x20  // 设置单价是否成功事件
#define TRANSACTION_END_EVENT_ID 0x40  // 交易结束事件
#define STATE_CHANGE_EVENT_ID 0x80
#define FAULT_MESSAGE_EVENT_ID 0x0100
#define SERIAL_ERROR_EVENT_ID 0x0200
#define GOODS_SOLD_OUT_STATE_EVENT_ID 0x0400

class protocol : protected serialPort {  // 通信协议类，继承于串口类
public :
    bool isVendingMachineAlive;
    
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
    protocol();

    protocol(bool isInit) {}

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
    virtual ~protocol();
    
    void initialization();
    
    void startProtocol();

    int startProtocol(char* portNameIn, unsigned int baudRateIn, bool isDebug);

    /*********************************************************************
    * @fn      resolveData
    *
    * @brief   解析通信协议
    *
    * @param   none
    *
    * @return  none
    */
    void resolveData();

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
    void getSelectedGoodInformation(unsigned short& amount, unsigned char& columnNo);

    unsigned short getSelectedGoodAmount();

    unsigned char getSelectedGoodColumnNo();

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
    void getCurrentSalesInformation(unsigned char& columnNo, unsigned char* salesCount, unsigned char* salesAmount);

    unsigned char getCurrentSalesColumnNo();

    void getCurrentSalesCount(unsigned char* salesCount);

    void getCurrentSalesAmount(unsigned char* salesAmount);
    
    void getGoodsSoldOutState(unsigned char* goodsSoldOutState);

    /*********************************************************************
    * @fn      cancelTransaction
    *
    * @brief   取消交易
    *
    * @param   none
    *
    * @return  none
    */
    void cancelTransaction();

    /*********************************************************************
    * @fn      outGoods
    *
    * @brief   按键非B0出货
    *
    * @param   none
    *
    * @return  none
    */
    void outGoods();

    /*********************************************************************
    * @fn      outGoodsUseB0ByButton
    *
    * @brief   按键B0出货
    *
    * @param   输入参数 salesCategory - 销售种类
    *
    * @return  none
    */
    void outGoodsUseB0ByButton(unsigned char salesCategory);

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
    void outGoodsUseB0(unsigned char columnNo, unsigned char salesCategory);

    /*********************************************************************
    * @fn      getColumnCount
    *
    * @brief   获取货道数量
    *
    * @param   none
    *
    * @return  货道数量
    */
    unsigned int const getColumnCount() const {return columnCount;}

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
    void setColumnPrice(unsigned char const* price);

    /*********************************************************************
    * @fn      getSetPriceResults
    *
    * @brief   获取单价设置是否成功
    *
    * @param   输出参数 instruction - 返回当前设置价格的指令
    *
    * @return  true：成功，false：失败
    */
    bool getSetPriceResults(unsigned char& instruction);

    bool getSetPriceResults();

    /*********************************************************************
    * @fn      getEvent
    *
    * @brief   获取事件id号
    *
    * @param   none
    *
    * @return  事件id号
    */
    unsigned int const getEvent();

    /*********************************************************************
    * @fn      selectGoodsByPC
    *
    * @brief   pc端选货
    *
    * @param   输入参数 columnNo - 货道号
    *
    * @return  none
    */
    void selectGoodsByScreen(unsigned char columnNo);
    
    /*********************************************************************
    * @fn      setEvent
    *
    * @brief   设置事件
    *
    * @param   输入参数 eventId - 事件id号
    *
    * @return  none
    */
    void setEvent(unsigned int eventId);
    
    unsigned char getStateData();
    
    unsigned int getFaultInformationCount();
    void getFaultInformation(unsigned char* faultBuf);
    void getVendingMachineNumber(unsigned char* vMN);
protected :
    unsigned char frame_receive[BUF_SZ];  // 数据接收buf
    
    unsigned char frame_buf[BUF_SZ];
    unsigned int index;
    unsigned int currentIndex;
    unsigned int frame_buf_len;
    
    unsigned char frame_send[BUF_SZ];  // 数据发送buf
    unsigned int frame_len_send;  // 待发送数据的长度
    
    unsigned char cyclicQueue[BUF_SZ];  // 循环队列
	unsigned int first, last;  // 队列头和尾的游标
	
    sem_t cyclicQueue_sem_id;  // 循环队列信号量
    sem_t event_sem_id;  // 事件信号量
    sem_t init_sem_id;  // 与售货机首次交互完成的标志

	unsigned short currentAmount;  // 当前交易的金额
	unsigned char currentColumnNo;  // 当前交易的货道号

    unsigned char READER_VER[2];  // 本程序的版本号
	unsigned char vendingMachineNumber[4];  // 售货机号
	unsigned char vendingMachineDate[8];  // 时钟信息
	unsigned char waitingTimeForTransaction;  // 接触等待时间

	unsigned char totalSalesCount[4];  // 总销售数量
	unsigned char totalSalesAmount[4];  // 总销售金额
	unsigned char totalSalesCountByCash[4];  // 现金总销售数量
	unsigned char totalSalesAmountByCash[4];  // 现金总销售金额
	unsigned char totalSalesCountByNotCash[4];  // 现金以外总销售数量
	unsigned char totalSalesAmountByNotCash[4];  // 现金以外总销售金额

	unsigned int columnCount;  // 货道数
	unsigned char saleCountsPerColumn[MAX_COLUMN_COUNT][4];  // 各个货道销售数量
	unsigned char saleAmountPerColumn[MAX_COLUMN_COUNT][4];  // 各个货道销售金额
	unsigned char saleSoldoutPerColumn[(MAX_COLUMN_COUNT + 7) / 8];  // 各个货道售空信息
	unsigned char beverageInformationPerColumn[MAX_COLUMN_COUNT][3];  // 各个货道的饮料代码信息
	unsigned char beveragePricePerColumn[MAX_COLUMN_COUNT][2];  // 各个货道的饮料单价
	unsigned char temperaturePerColumn[MAX_COLUMN_COUNT];  // 各个货道的温度设定
	unsigned char beverageCardPricePerColumn[MAX_COLUMN_COUNT][2];  // 各个货道的饮料卡单价

	unsigned char faultInformation[10][2];  // 故障信息
	unsigned int faultInformationCount;  // 故障数

	unsigned char stateData;  // 售货机状态数据

	unsigned char vmcFirmwareVersion[3];  // 售货机主控程序编号

	unsigned char vmcSettingData;  // 售货机设定数据

	unsigned char processingInstruction;  // 当前结果对应的指令
	unsigned char processedResult;  // 指令处理结果

	timeval starttime_transaction;  // 交易超时计时

    bool isTransactionProcessing;  // 交易是否正在处理
	bool isAfterSettlementData;  // 是否是在结算后，用于判断是回0x5B01,还是0x5B03
	
	unsigned int eventBit;

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
    void addReaderControllerStateData(bool isCardAccepted, bool isTransactionCancelled, bool isInProcessing);

    /*********************************************************************
    * @fn      generateData
    *
    * @brief   生成数据包，包括添加起始，结束，长度，校验字段
    *
    * @param   none
    *
    * @return  none
    */
    void generateData();

    /*********************************************************************
    * @fn      addDc
    *
    * @brief   添加数据DC
    *
    * @param   输入参数 dc - 数据DC
    *
    * @return  none
    */
    void addDc(unsigned char dc);

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
    void addData(unsigned char const* buf, unsigned int size);

    /*********************************************************************
    * @fn      checkInstructions
    *
    * @brief   检查指令格式
    *
    * @param   none
    *
    * @return  true：指令格式正确，false：指令格式错误
    */
    bool const checkInstructions();

    /*********************************************************************
    * @fn      returnAck
    *
    * @brief   返回ACK
    *
    * @param   输入参数 ack - ack码
    *
    * @return  none
    */
    void returnAck(unsigned char ack);

    /*********************************************************************
    * @fn      setFCC
    *
    * @brief   设置校验字段
    *
    * @param   none
    *
    * @return  none
    */
    void setFCC();

    /*********************************************************************
    * @fn      addReaderControllerFixedData
    *
    * @brief   添加固定数据 DC = 0x4D
    *
    * @param   none
    *
    * @return  none
    */
    void addReaderControllerFixedData();

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
    void addReaderControllerExceptionData(bool isReaderException, bool isReaderNoResponse,
                                          bool isReaderCommunicationException, bool isReaderInitializationException);

    /*********************************************************************
    * @fn      checkData
    *
    * @brief   检查数据格式和校验是否正确
    *
    * @param   none
    *
    * @return  true：格式和校验正确，false：格式或校验错误
    */
    bool const checkData();

    /*********************************************************************
    * @fn      resolveDC
    *
    * @brief   解析，存储，更新售货机返回的数据，包括交易数据，控制数据，货道状况，故障信息，单价，温度，状态，指令执行结果
    *
    * @param   none
    *
    * @return  true ： 解析成功，false ： 有无法识别的DC
    */
    bool const resolveDC();

    /*********************************************************************
    * @fn      addSettlementData
    *
    * @brief   添加结算数据
    *
    * @param   none
    *
    * @return  none
    */
    void addSettlementData();

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
    void toBCD(unsigned int amount, unsigned char* buf) const;

    /*********************************************************************
    * @fn      getSharedMemoryBufLength
    *
    * @brief   获取循环队列的数据长度
    *
    * @param   none
    *
    * @return  数据长度
    */
    unsigned int const getCyclicQueueLength() const {return first > last ? BUF_SZ - (first - last) : last - first;}

    /*********************************************************************
    * @fn      isSharedMemoryBufEmpty
    *
    * @brief   循环队列是否为空
    *
    * @param   none
    *
    * @return  true：空，false：有数据
    */
    bool const isCyclicQueueEmpty() const {return first == last;}

    /*********************************************************************
    * @fn      pushSharedMemoryBuf
    *
    * @brief   数据写入循环队列，数据会在INPUT_REQUEST_INSTRUCTION指令里被发送
    *
    * @param   none
    *
    * @return  none
    */
    void pushCyclicQueue();

    /*********************************************************************
    * @fn      popSharedMemoryBuf
    *
    * @brief   从循环队列里弹出数据
    *
    * @param   none
    *
    * @return  none
    */
    void popCyclicQueue();

    /*********************************************************************
    * @fn      processingPayment
    *
    * @brief   开始结算
    *
    * @param   none
    *
    * @return  none
    */
    void processingPayment();
};

extern protocol vendingMachineProtocol;

#endif //MYAPPLICATION_PROTOCOL_H
