#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include <ctime>
#include <cstdlib>
#include "unistd.h"
#include "UsbCanBus.h"

using namespace std;

UsbCanBus::UsbCanBus()
{
    canind_ = 0;
    config.AccCode = 0;
    config.AccMask = 0xFFFFFFFF;
    config.Filter = 1;            // 接收所有帧
    config.Timing0 = BR500k >> 8; /*波特率1000 Kbps  0x00  0x14*/
    config.Timing1 = BR100k & 0xff;
    config.Mode = 0; // 0正常模式 1监控模式 2环回模式

    openStatus = this->Open();
    this->Start();
}

UsbCanBus::UsbCanBus(uint8_t canind, Bit_Rate rate)
{
    canind_ = canind;
    config.AccCode = 0;
    config.AccMask = 0xFFFFFFFF;
    config.Filter = 1;          // 接收所有帧
    config.Timing0 = rate >> 8; /*波特率1000 Kbps  0x00  0x14*/
    config.Timing1 = rate & 0xff;
    config.Mode = 0; // 0正常模式 1监控模式 2环回模式

    this->Open();
    this->Start();
}

UsbCanBus::~UsbCanBus()
{
    ::VCI_ResetCAN(VCI_USBCAN2, 0, 0); // 复位CAN1通道。
    usleep(100000);                    // 延时100ms。
    ::VCI_ResetCAN(VCI_USBCAN2, 0, 1); // 复位CAN2通道。
    usleep(100000);                    // 延时100ms。
    ::VCI_CloseDevice(VCI_USBCAN2, 0); // 关闭设备。
}

int UsbCanBus::Open()
{
    int ret = 0;
    if ((ret = ::VCI_OpenDevice(VCI_USBCAN2, 0, 0)) == -1) // 打开设备
    {
        perror(">>open deivce error!\n");
        sleep(1);
        return -1;
    }
    cout << "open deivce ok!" << endl;

    if (VCI_InitCAN(VCI_USBCAN2, 0, 0, &config) != 1)
    {
        perror(">>Init CAN1 error\n");
        VCI_CloseDevice(VCI_USBCAN2, 0);
    }
    if (VCI_StartCAN(VCI_USBCAN2, 0, 0) != 1)
    {
        perror(">>Start CAN1 error\n");
        VCI_CloseDevice(VCI_USBCAN2, 0);
    }
    if (VCI_InitCAN(VCI_USBCAN2, 0, 1, &config) != 1)
    {
        perror(">>Init can2 error\n");
        VCI_CloseDevice(VCI_USBCAN2, 0);
    }
    if (VCI_StartCAN(VCI_USBCAN2, 0, 1) != 1)
    {
        perror(">>Start can2 error\n");
        VCI_CloseDevice(VCI_USBCAN2, 0);
    }
    VCI_ReadCANStatus(VCI_USBCAN2, 0, 0, &status[0]);
    VCI_ReadCANStatus(VCI_USBCAN2, 0, 1, &status[1]);
    // sleep(2);//延时100ms。
    printf("Can Init ok!\r\n");
    return 0;
}

void UsbCanBus::Close()
{
    ::VCI_ResetCAN(VCI_USBCAN2, 0, 0); // 复位CAN1通道。
    usleep(100000);                    // 延时100ms。
    ::VCI_ResetCAN(VCI_USBCAN2, 0, 1); // 复位CAN2通道。
    usleep(100000);                    // 延时100ms。
    ::VCI_CloseDevice(VCI_USBCAN2, 0); // 关闭设备。
}

ssize_t UsbCanBus::Transmit(CanTxMsg *TxMessage, unsigned int len)
{
    ssize_t ret = 0;
    VCI_CAN_OBJ *send = new VCI_CAN_OBJ[len];
    for (unsigned int i = 0; i < len; i++)
    {
        send[i].SendType = 1; // 0自动重发，1只发送一次
        send[i].DataLen = TxMessage[i].DLC;
        if (TxMessage[i].IDE == CAN_ID_STD)
        {
            send[i].ExternFlag = 0; // 是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
            send[i].ID = TxMessage[i].StdId;
        }
        else if (TxMessage[i].IDE == CAN_ID_EXT)
        {
            send[i].ExternFlag = 1; // 是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
            send[i].ID = TxMessage[i].ExtId;
        }
        else
        {
            perror(">>TxMessage[i].IDE error\n");
            return -1;
        }
        if (TxMessage[i].RTR == CAN_RTR_DATA)
        {
            send[i].RemoteFlag = 0; // 是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
            for (int j = 0; j < send[i].DataLen; j++)
            {
                send[i].Data[j] = TxMessage[i].Data[j];
            }
        }
        else if (TxMessage[i].RTR == CAN_RTR_REMOTE)
        {
            send[i].DataLen = 0;
            send[i].RemoteFlag = 1; // 是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
        }
        else
        {
            perror(">>TxMessage->RTR error\n");
            return -1;
        }
        //printf("TXmsg ID:0x%4x LEN:%2d data: %02x %02x %02x %02x %02x %02x %02x %02x \r\n",send[i].ID,send[i].DataLen,send[i].Data[0] \
                ,send[i].Data[1] ,send[i].Data[2] ,send[i].Data[3] ,send[i].Data[4] ,send[i].Data[5] ,send[i].Data[6] ,send[i].Data[7] );
    }
    if ((ret = VCI_Transmit(VCI_USBCAN2, 0, canind_, send, len)) == 1)
    {
        // printf("ret = %d\n", ret);
        delete[] send;
        return 0;
    }
    perror(">>VCI_Transmit error\n");
    delete[] send;
    return -1;
}

ssize_t UsbCanBus::Receive(CanRxMsg *RxMessage, uint32_t length)
{
    VCI_CAN_OBJ *mes = (VCI_CAN_OBJ *)malloc(sizeof(VCI_CAN_OBJ) * length);
    if (mes == NULL)
    {
        perror("usbcan malloc eeror");
        return 0;
    }

    int reclen = (int)VCI_Receive(VCI_USBCAN2, 0, canind_, mes, length, 100);
    if (reclen >= 0) // 调用接收函数，如果有数据，进行数据处理显示。
    {
        if (reclen == 0)
        {
            if (VCI_ReadCANStatus(VCI_USBCAN2, 0, 0, &status[0]) == 0 ||
                VCI_ReadCANStatus(VCI_USBCAN2, 0, 1, &status[1]) == 0)
            {
            }
        }
        for (int j = 0; j < reclen; j++)
        {
            if (mes[j].ExternFlag == CAN_ID_STD)
            {
                RxMessage[j].IDE = CAN_ID_STD;
                RxMessage[j].StdId = mes[j].ID;
                RxMessage[j].ExtId = 0;
            }
            else if (mes[j].ExternFlag == CAN_ID_EXT)
            {
                RxMessage[j].IDE = CAN_ID_EXT;
                RxMessage[j].StdId = 0;
                RxMessage[j].ExtId = mes[j].ID;
            }

            if (mes[j].RemoteFlag == CAN_RTR_DATA)
            {
                RxMessage[j].RTR = CAN_RTR_DATA;
                RxMessage[j].DLC = mes[j].DataLen;
                for (int i = 0; i < RxMessage[j].DLC; i++)
                {
                    RxMessage[j].Data[i] = mes[j].Data[i];
                }
            }
            else if (mes[j].RemoteFlag == CAN_RTR_REMOTE)
            {
                RxMessage[j].RTR = CAN_RTR_REMOTE;
            }
        }
        free(mes);
        return reclen;
    }
    else
    {
        printf("Can 设备不在线! return %d\n", reclen);
        Open();
        free(mes);
    }
    return reclen;
}

UsbCan2Bus::UsbCan2Bus()
{
    canind_ = 1;
    config.AccCode = 0;
    config.AccMask = 0xFFFFFFFF;
    config.Filter = 1;            // 接收所有帧
    config.Timing0 = BR500k >> 8; /*波特率1000 Kbps  0x00  0x14*/
    config.Timing1 = BR100k & 0xff;
    config.Mode = 0; // 0正常模式 1监控模式 2环回模式

    // this->Open();
    this->Start();
}

UsbCan2Bus::UsbCan2Bus(uint8_t canind, Bit_Rate rate)
{
    canind_ = canind;
    config.AccCode = 0;
    config.AccMask = 0xFFFFFFFF;
    config.Filter = 1;          // 接收所有帧
    config.Timing0 = rate >> 8; /*波特率1000 Kbps  0x00  0x14*/
    config.Timing1 = rate & 0xff;
    config.Mode = 0; // 0正常模式 1监控模式 2环回模式

    // this->Open();
    this->Start();
}

UsbCan2Bus::~UsbCan2Bus()
{
    ::VCI_ResetCAN(VCI_USBCAN2, 0, 0); // 复位CAN1通道。
    usleep(100000);                    // 延时100ms。
    ::VCI_ResetCAN(VCI_USBCAN2, 0, 1); // 复位CAN2通道。
    usleep(100000);                    // 延时100ms。
    ::VCI_CloseDevice(VCI_USBCAN2, 0); // 关闭设备。
}

int UsbCan2Bus::Open()
{
    if ((::VCI_OpenDevice(VCI_USBCAN2, 0, 0)) == -1) // 打开设备
    {
        perror(">>open deivce error!\n");
        sleep(1);
        return -1;
    }
    cout << "open deivce ok!" << endl;

    if (VCI_InitCAN(VCI_USBCAN2, 0, 0, &config) != 1)
    {
        perror(">>Init CAN1 error\n");
        VCI_CloseDevice(VCI_USBCAN2, 0);
    }
    if (VCI_StartCAN(VCI_USBCAN2, 0, 0) != 1)
    {
        perror(">>Start CAN1 error\n");
        VCI_CloseDevice(VCI_USBCAN2, 0);
    }
    if (VCI_InitCAN(VCI_USBCAN2, 0, 1, &config) != 1)
    {
        perror(">>Init can2 error\n");
        VCI_CloseDevice(VCI_USBCAN2, 0);
    }
    if (VCI_StartCAN(VCI_USBCAN2, 0, 1) != 1)
    {
        perror(">>Start can2 error\n");
        VCI_CloseDevice(VCI_USBCAN2, 0);
    }
    VCI_ReadCANStatus(VCI_USBCAN2, 0, 0, &status[0]);
    VCI_ReadCANStatus(VCI_USBCAN2, 0, 1, &status[1]);
    // sleep(2);//延时100ms。
    printf("Can Init ok!\r\n");
    return 0;
}

void UsbCan2Bus::Close()
{
    ::VCI_ResetCAN(VCI_USBCAN2, 0, 0); // 复位CAN1通道。
    usleep(100000);                    // 延时100ms。
    ::VCI_ResetCAN(VCI_USBCAN2, 0, 1); // 复位CAN2通道。
    usleep(100000);                    // 延时100ms。
    ::VCI_CloseDevice(VCI_USBCAN2, 0); // 关闭设备。
}

ssize_t UsbCan2Bus::Transmit(CanTxMsg *TxMessage, unsigned int len)
{
    ssize_t ret = 0;
    VCI_CAN_OBJ *send = new VCI_CAN_OBJ[len];
    for (unsigned int i = 0; i < len; i++)
    {
        send[i].SendType = 1; // 0自动重发，1只发送一次
        send[i].DataLen = TxMessage[i].DLC;
        if (TxMessage[i].IDE == CAN_ID_STD)
        {
            send[i].ExternFlag = 0; // 是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
            send[i].ID = TxMessage[i].StdId;
        }
        else if (TxMessage[i].IDE == CAN_ID_EXT)
        {
            send[i].ExternFlag = 1; // 是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
            send[i].ID = TxMessage[i].ExtId;
        }
        else
        {
            perror(">>TxMessage[i].IDE error\n");
            return -1;
        }
        if (TxMessage[i].RTR == CAN_RTR_DATA)
        {
            send[i].RemoteFlag = 0; // 是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
            for (int j = 0; j < send[i].DataLen; j++)
            {
                send[i].Data[j] = TxMessage[i].Data[j];
            }
        }
        else if (TxMessage[i].RTR == CAN_RTR_REMOTE)
        {
            send[i].DataLen = 0;
            send[i].RemoteFlag = 1; // 是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
        }
        else
        {
            perror(">>TxMessage->RTR error\n");
            return -1;
        }
        //printf("TXmsg ID:0x%4x LEN:%2d data: %02x %02x %02x %02x %02x %02x %02x %02x \r\n",send[i].ID,send[i].DataLen,send[i].Data[0] \
                ,send[i].Data[1] ,send[i].Data[2] ,send[i].Data[3] ,send[i].Data[4] ,send[i].Data[5] ,send[i].Data[6] ,send[i].Data[7] );
    }
    if ((ret = VCI_Transmit(VCI_USBCAN2, 0, canind_, send, len)) == 1)
    {
        delete[] send;
        return 0;
    }
    perror(">>VCI_Transmit error\n");
    delete[] send;
    return -1;
}

ssize_t UsbCan2Bus::Receive(CanRxMsg *RxMessage, uint32_t length)
{
    VCI_CAN_OBJ *mes = (VCI_CAN_OBJ *)malloc(sizeof(VCI_CAN_OBJ) * length);
    if (mes == NULL)
    {
        perror("usbcan malloc eeror");
        return 0;
    }

    int reclen = (int)VCI_Receive(VCI_USBCAN2, 0, canind_, mes, length, 100);
    if (reclen >= 0) // 调用接收函数，如果有数据，进行数据处理显示。
    {
        if (reclen == 0)
        {
            if (VCI_ReadCANStatus(VCI_USBCAN2, 0, 0, &status[0]) == 0 ||
                VCI_ReadCANStatus(VCI_USBCAN2, 0, 1, &status[1]) == 0)
            {
            }
        }
        for (int j = 0; j < reclen; j++)
        {
            if (mes[j].ExternFlag == CAN_ID_STD)
            {
                RxMessage[j].IDE = CAN_ID_STD;
                RxMessage[j].StdId = mes[j].ID;
                RxMessage[j].ExtId = 0;
            }
            else if (mes[j].ExternFlag == CAN_ID_EXT)
            {
                RxMessage[j].IDE = CAN_ID_EXT;
                RxMessage[j].StdId = 0;
                RxMessage[j].ExtId = mes[j].ID;
            }

            if (mes[j].RemoteFlag == CAN_RTR_DATA)
            {
                RxMessage[j].RTR = CAN_RTR_DATA;
                RxMessage[j].DLC = mes[j].DataLen;
                for (int i = 0; i < RxMessage[j].DLC; i++)
                {
                    RxMessage[j].Data[i] = mes[j].Data[i];
                }
            }
            else if (mes[j].RemoteFlag == CAN_RTR_REMOTE)
            {
                RxMessage[j].RTR = CAN_RTR_REMOTE;
            }
        }
        free(mes);
        return reclen;
    }
    else
    {
        printf("Can 设备不在线! return %d\n", reclen);
        // Open();
        free(mes);
    }
    return reclen;
}
