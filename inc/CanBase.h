#ifndef _CAN_BASE__H
#define _CAN_BASE__H

#include <stdbool.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <map>
#include<mutex>

using namespace std;
template <typename T>
int PthreadCreate(pthread_t *thread_id, const pthread_attr_t *attr, void *(T::*callback)(), T *arg)
{
    void *(*__start_routine)(void *);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
    __start_routine = (void *(*)(void *))callback;
#pragma GCC diagnostic pop
    return pthread_create(thread_id, attr, __start_routine, (void *)arg);
}
// ===================================================================================
class CanBase
{
public:
    enum RtrRate
    {
        CAN_RTR_DATA = 0x00,   /*!< Data frame */
        CAN_RTR_REMOTE = 0x01, /*!< Remote frame */
    };

    enum IdeRate
    {
        CAN_ID_STD = 0x00, /*!< Standard Id */
        CAN_ID_EXT = 0x01, /*!< Extended Id */
    };

    /**
     * @brief  CAN Tx message structure definition
     */
    typedef struct
    {
        uint32_t StdId; /*!< Specifies the standard identifier.
                               This parameter can be a value between 0 to 0x7FF. */

        uint32_t ExtId; /*!< Specifies the extended identifier.
                               This parameter can be a value between 0 to 0x1FFFFFFF. */

        IdeRate IDE; /*!< Specifies the type of identifier for the message that
                            will be transmitted. This parameter can be a value
                            of @ref CAN_identifier_type */

        RtrRate RTR; /*!< Specifies the type of frame for the message that will
                            be transmitted. This parameter can be a value of
                            @ref CAN_remote_transmission_request */

        uint8_t DLC = 0; /*!< Specifies the length of the frame that will be
                            transmitted. This parameter can be a value between
                            0 to 8 */

        uint8_t Data[8]; /*!< Contains the data to be transmitted. It ranges from 0
                                to 0xFF. */
    } CanTxMsg;

    /**
     * @brief  CAN Rx message structure definition
     */
    typedef struct
    {
        uint32_t StdId; /*!< Specifies the standard identifier.
                            This parameter can be a value between 0 to 0x7FF. */

        uint32_t ExtId; /*!< Specifies the extended identifier.
                            This parameter can be a value between 0 to 0x1FFFFFFF. */

        IdeRate IDE; /*!< Specifies the type of identifier for the message that
                         will be received. This parameter can be a value of
                         @ref CAN_identifier_type */

        RtrRate RTR; /*!< Specifies the type of frame for the received message.
                         This parameter can be a value of
                         @ref CAN_remote_transmission_request */

        uint8_t DLC; /*!< Specifies the length of the frame that will be received.
                         This parameter can be a value between 0 to 8 */

        uint8_t Data[8]; /*!< Contains the data to be received. It ranges from 0 to
                             0xFF. */

        uint8_t FMI; /*!< Specifies the index of the filter the message stored in
                         the mailbox passes through. This parameter can be a
                         value between 0 to 0xFF */
    } CanRxMsg;

    typedef struct
    {
        uint32_t bus_open_failure : 1;   // CanBase Open Failure
        uint32_t no_response : 1;        // Data sent, but no feedback
        uint32_t protocol_exception : 1; // The format of the feedback data is wrong, or the verification fails
    } error_code_t;

    typedef struct
    {
        uint16_t can_tx_interval_cur_s = 0;
        uint16_t can_tx_interval_max_s = 0;
        uint16_t can_rx_interval_cur_s = 0;
        uint16_t can_rx_interval_max_s = 0;
    } can_tx_rx_info_t;

    std::map<uint32_t, CanTxMsg> tx_msg_list;
    std::map<uint32_t, CanRxMsg> rx_msg_list;

private:
    typedef struct
    {
        uint32_t std_id;
        void *this_p;
        void (*callback)(void *, CanRxMsg);
    } can_bus_callback_t;

    vector<can_bus_callback_t> can_bus_callback_list_;
    std::mutex mtx;
    pthread_t thread_ = -1;
    bool run_flag = true;

private:
    void *runThread();
    void canReceiveRun(void);

protected:
public:
    CanBase(void);
    virtual ~CanBase(void);
    virtual int32_t getOpenStatus(){ return 0;}
    void Start();

    uint8_t SDO_Write(uint16_t node_id, int index, uint32_t value);
    uint8_t SDO_Write(uint16_t node_id, uint16_t index, uint8_t sub_index, uint32_t value, uint8_t len);
    uint8_t SDO_Read(uint16_t node_id, int index);
    ssize_t Transmit(uint32_t id, uint8_t *data, uint8_t);
    ssize_t Transmit(CanTxMsg &TxMessage);

    can_tx_rx_info_t can_tx_rx_info;

    /**
     *
     *
     */
    template <class T>
    void CanReceiveRegister(uint32_t id, void (T::*callback)(CanRxMsg), T *this_p)
    {
        can_bus_callback_t can_bus_callback;
        can_bus_callback.std_id = id;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
        can_bus_callback.callback = (void (*)(void *, CanRxMsg))callback;
#pragma GCC diagnostic pop

        can_bus_callback.this_p = this_p;
        std::lock_guard<std::mutex> lock(mtx);
        can_bus_callback_list_.push_back(can_bus_callback);
    };

    virtual ssize_t Transmit(CanTxMsg *TxMessage, unsigned int Len)
    { /*printf("virtual ssize_t Transmit \r\n");*/
        return -1;
    };
    virtual ssize_t Receive(CanRxMsg *RxMessage, uint32_t)
    {
        printf("virtual ssize_t Receive \r\n");
        return -1;
    };
    virtual void GetErrorCode(error_code_t &){};
    /**
     *
     *
     */
    virtual void CanRegisterZeroSpeedFrame(CanTxMsg &TxMessage, uint16_t keep_interval_tick, uint16_t max_interval_tick){};
};

class CanBusDemo : public CanBase
{
public:
public:
    CanBusDemo(void){};
    ~CanBusDemo(void){};

    ssize_t Transmit(CanTxMsg *TxMessage, unsigned int len) { printf("virtual ssize_t Transmit"); return len;};
    ssize_t Receive(CanRxMsg *RxMessage, uint32_t len) { printf("virtual ssize_t Receive"); return len;}

protected:
private:
};

#endif // CAN_H