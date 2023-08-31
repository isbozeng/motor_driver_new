#include "CanBase.h"
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h> /* For SYS_xxx definitions */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

CanBase::CanBase(void):run_flag(true)
{
}

CanBase::~CanBase(void)
{
    run_flag = false;
    if (thread_ != -1)
    {
        pthread_join(thread_, NULL);
    }
    printf("~CanBase \r\n");
}

void CanBase::Start()
{
    int ret_thrd = PthreadCreate(&thread_, NULL, &CanBase::runThread, this);
    if (ret_thrd != 0)
    {
        cout << "Thread creation failed\n"
             << endl;
    }
}

/*
if(msg.StdId >= 0x580 && msg.StdId < 0x600){
    uint16_t nodeid    = msg.StdId - 0x580;
    uint8_t  cmd       = msg.Data[0];
    uint16_t index     = *(uint16_t*)&msg.Data[1];
    uint8_t  sub_index = msg.Data[3];
    for(uint8_t i = 0; i < registered_can1_sdo_irq_num ; i++){
        if(nodeid == can1_sdo_irq_node[i].nodeid && index == can1_sdo_irq_node[i].index &&
            sub_index == can1_sdo_irq_node[i].sub_index ){
            switch (cmd) {
                case 0x4f: can1_sdo_irq_node[i].irq_hook(nodeid,index,sub_index,*(uint8_t* )&msg.Data[4]);break;
                case 0x4b: can1_sdo_irq_node[i].irq_hook(nodeid,index,sub_index,*(uint16_t*)&msg.Data[4]);break;
                case 0x43: can1_sdo_irq_node[i].irq_hook(nodeid,index,sub_index,*(uint32_t*)&msg.Data[4]);break;
                case 0x60: can1_sdo_irq_node[i].irq_hook(nodeid,index,sub_index,*(uint32_t*)&msg.Data[4]);break;
                default: break;
            }
        }
    }
}
*/
ssize_t CanBase::Transmit(uint32_t id, uint8_t *data, uint8_t length)
{
    CanTxMsg TxMessage;
    TxMessage.StdId = id;
    TxMessage.IDE = CAN_ID_STD;
    TxMessage.DLC = length;
    TxMessage.RTR = CAN_RTR_DATA; //(length != 0) ? CAN_RTR_DATA : CAN_RTR_REMOTE;
    for (int i = 0; i < length; i++)
    {
        TxMessage.Data[i] = data[i];
    }
    tx_msg_list[id] = TxMessage;
    return Transmit(&TxMessage, 1);
}

ssize_t CanBase::Transmit(CanTxMsg &TxMessage)
{
    tx_msg_list[TxMessage.StdId] = TxMessage;
    return Transmit(&TxMessage, 1);
}

void CanBase::canReceiveRun(void)
{
    CanRxMsg RxMessage[100];

    ssize_t len = Receive(RxMessage, 100);
    if (len > 0)
    {
        for (int i = 0; i < len; i++)
        {
            if (RxMessage[i].IDE != CAN_ID_STD && RxMessage[i].IDE != CAN_ID_EXT)
            {
                // //LOG(ERROR) << "can mes frame formate not stdID or extid , IDE = " << RxMessage[i].IDE;
            }
            else if (RxMessage[i].DLC > 8)
            {
                // LOG(ERROR) << "can mes DLC > 8 , DLC = " << (int)RxMessage[i].DLC;
            }
            else
            {
                if (RxMessage[i].IDE == CAN_ID_EXT)
                {
                    RxMessage[i].StdId = RxMessage[i].ExtId;
                }
                for (int n = 0; n < can_bus_callback_list_.size(); n++)
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    if (RxMessage[i].IDE == CAN_ID_STD)
                    {
                        RxMessage[i].ExtId = 0;
                    }
                    else if (RxMessage[i].IDE == CAN_ID_EXT)
                    {
                        RxMessage[i].StdId = 0;
                    }
                    if (RxMessage[i].StdId == can_bus_callback_list_[n].std_id || RxMessage[i].ExtId == can_bus_callback_list_[n].std_id)
                    {
                        can_bus_callback_list_[n].callback(can_bus_callback_list_[n].this_p, RxMessage[i]);
                    }
                }
                if (RxMessage[i].IDE == CAN_ID_STD)
                {
                    RxMessage[i].ExtId = 0;
                    rx_msg_list[RxMessage[i].StdId] = RxMessage[i];
                }
                else if (RxMessage[i].IDE == CAN_ID_EXT)
                {
                    RxMessage[i].StdId = 0;
                    rx_msg_list[RxMessage[i].ExtId] = RxMessage[i];
                }
            }
        }
    }
}

uint8_t CanBase::SDO_Write(uint16_t node_id, int index, uint32_t value)
{
    CanBase::CanTxMsg msg;
    msg.StdId = 0x600 | node_id; // 0x600 is the SDO identifier
    msg.ExtId = 0;
    msg.IDE = CanBase::CAN_ID_STD;
    msg.RTR = CanBase::CAN_RTR_DATA;
    msg.DLC = 8;

    if ((index & 0x000000ff) == 0x08)
    {
        msg.Data[0] = 0x2F;
    }
    else if ((index & 0x000000ff) == 0x10)
    {
        msg.Data[0] = 0x2B;
    }
    else if ((index & 0x000000ff) == 0x20)
    {
        msg.Data[0] = 0x23;
    }
    else
    {
        msg.Data[0] = 0x22;
    }
    msg.Data[1] = (index >> 16) & 0xff;
    msg.Data[2] = (index >> 24) & 0xff;
    msg.Data[3] = (index >> 8) & 0xff;
    *(uint32_t *)&msg.Data[4] = value;
    return Transmit(msg);
}

uint8_t CanBase::SDO_Write(uint16_t node_id, uint16_t index, uint8_t sub_index, uint32_t value, uint8_t len)
{
    CanBase::CanTxMsg msg;
    msg.StdId = 0x600 | node_id; // 0x600 is the SDO identifier
    msg.ExtId = 0;
    msg.IDE = CanBase::CAN_ID_STD;
    msg.RTR = CanBase::CAN_RTR_DATA;
    msg.DLC = 8;

    switch (len)
    {
    case 1:
        msg.Data[0] = 0x2F;
        break;
    case 2:
        msg.Data[0] = 0x2B;
        break;
    case 3:
        msg.Data[0] = 0x27;
        break;
    case 4:
        msg.Data[0] = 0x23;
        break;
    default:
        msg.Data[0] = 0x23;
        break;
    }

    msg.Data[1] = index;
    msg.Data[2] = (index >> 8) & 0xff;
    msg.Data[3] = sub_index;
    *(uint32_t *)&msg.Data[4] = value;
    return Transmit(msg);
}

uint8_t CanBase::SDO_Read(uint16_t node_id, int index)
{
    CanBase::CanTxMsg msg;
    msg.StdId = 0x600 | node_id; // 0x600 is the SDO identifier
    msg.ExtId = 0;
    msg.IDE = CanBase::CAN_ID_STD;
    msg.RTR = CanBase::CAN_RTR_DATA;
    msg.DLC = 4;

    msg.Data[0] = 0x40;
    msg.Data[1] = (index >> 16) & 0xff;
    msg.Data[2] = (index >> 24) & 0xff;
    msg.Data[3] = (index >> 8) & 0xff;

    return Transmit(msg);
}

void *CanBase::runThread()
{
    // printf("CanBase::runThread() tid = %lu\n", syscall(SYS_gettid));
    // B//LOG(INFO, BLOG_LEVEL_DEFAULT, " CanBase::runThread tid: ", (int)syscall(SYS_gettid));

    while (run_flag)
    {
        canReceiveRun();
        usleep(5000);
    }
    return NULL;
}
