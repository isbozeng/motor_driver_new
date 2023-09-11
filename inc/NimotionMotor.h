#pragma once
#include <unistd.h>
#include <stdint.h>
#include <string>
#include "ctrl_motor_interface.h"
#include "UsbCanBus.h"



class nimotionMotor : public CtrlMotorInterface
{
public:
    nimotionMotor(uint32_t id, bool inverse = false, uint32_t res = 10000);
    virtual ~nimotionMotor();
    virtual int32_t init() override;//电机初始化
    virtual int32_t setMotorPosition(double pos) override;//设置电机位置角度,单位rad
    virtual int32_t getMotorPosition(double& pos) override;//读取当前电机位置角度，单位rad,电机出错反馈错误码
    virtual int32_t setEnableStatus(bool enable) override;//设置上下使能
    virtual int32_t getEnableStatus(bool& enable) override;//获得当前电机使能状态，在发送一条轨迹前需要先判断电机是否已经使能
    virtual int32_t setHome(uint32_t timeout) override; //清0,将当前位置设置为零点

    /*以下几个是为了后期碰撞检测按读取力矩处理*/
    virtual int32_t getTorque(double& torque) override;//获得当前时刻电机力矩 单位:N*m
    virtual int32_t getVelocity(double& velocity) override;//获取当前时刻的速度(可选)  单位:rad/s
private:
    CanBase *can_bus_ = nullptr;
    uint32_t timeout = 3; // s

private:
#pragma pack(1)
    typedef struct
    {
        uint16_t switchOn : 1;
        uint16_t enableVol : 1;
        uint16_t quickStop : 1;
        uint16_t enableOperation : 1;
        uint16_t operationMode : 3;
        uint16_t faultReset : 1;
        uint16_t halt : 1;
        uint16_t : 7;
    } controlword_t;
#pragma pack()
#pragma pack(1)
    typedef struct
    {
        /* uint16_t :2;
        uint16_t operationMode:2;
        uint16_t internalLimitActive:1;
        uint16_t targetReached:1;
        uint16_t remote:1;
        uint16_t :1;
        uint16_t warning:1;
        uint16_t disswitchOn:1;
        uint16_t quickStop:1;
        uint16_t enableVol:1;
        uint16_t halt:1;
        uint16_t enableOperation:1;
        uint16_t switchOn:1;
        uint16_t ready:1;*/

        uint16_t ready : 1;
        uint16_t switchOn : 1;
        uint16_t enableOperation : 1;
        uint16_t halt : 1;
        uint16_t enableVol : 1;
        uint16_t quickStop : 1;
        uint16_t disswitchOn : 1;
        uint16_t warning : 1;
        uint16_t : 1;
        uint16_t remote : 1;
        uint16_t targetReached : 1;
        uint16_t internalLimitActive : 1;
        uint16_t operationMode : 2;
        uint16_t : 2;

    } statusword_t;
#pragma pack()
    enum NI_MOTION_STATUS_t
    {
        START_NODE,
        READY,
        SWITCH_ON,
        HOMING,
        CLEAR_ERROR,
        SET_MODE,
        SET_ENABLE,
        SET_CMD,
        MOTION_ENABLE,
        SET_POS_CMD,
        SET_VEL_CMD,
        MOTION,
        DONE
    };
    enum NI_MOTION_MODE_t
    {
        NULL_MODE = 0,
        POS_MODE,
        VEL_MODE,
        IP_MODE = 0x07,
    };
    enum SDO_ACK_t
    {
        NULL_ACK,
        POS_ACK,
        VEL_ACK,
    };
    enum CLEAR_POS_ACK_t
    {
        NULL_CLEAR,
        HIGHT_CLEAR,
        LOW_CLEAR
    };
    struct posCtlCmd
    {
        int32_t pos;
        int32_t vel;
        posCtlCmd(int32_t p, int32_t v) : pos(p), vel(v) {}
    };
    int16_t sixForce;
    int16_t current;
    uint16_t error_code = 0;
    int32_t pos_cmd = 0;
    int32_t vel_cmd = 0;
    int32_t cur_vel = 0;
    int32_t cur_pos = 0;
    uint32_t det_pos = 100;
    uint32_t resolution = 10000;
    double reduction = 1.0;
    double angle = 0.0;
    bool isOnline = false;
    bool isEnable = false;
    bool isRecPos = false;

    bool homeCmd = false;
    bool enableCmd = false;
    bool resetCmd = false;
    bool isNewCmd = false;
    NI_MOTION_MODE_t modeCmd = IP_MODE;
    NI_MOTION_STATUS_t nimotion_state = START_NODE;
    NI_MOTION_MODE_t nimotion_mode = NULL_MODE;
    SDO_ACK_t sdo_ack = NULL_ACK;
    CLEAR_POS_ACK_t clear_ack = NULL_CLEAR;
    statusword_t statusword;
    controlword_t controlword;
    std::mutex mtx;
    std::chrono::time_point<std::chrono::steady_clock> last_time;
    std::chrono::time_point<std::chrono::steady_clock> now_time;
    // std::queue<posCtlCmd> CmdQueue;

    void switchState();
    void run();
    int32_t getError();
    void recMsgCallback(CanBase::CanRxMsg msg);

};