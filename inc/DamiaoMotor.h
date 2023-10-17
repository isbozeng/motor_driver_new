#pragma once
#include "ctrl_motor_interface.h"
#include "UsbCanBus.h"
class damiaoMotor : public CtrlMotorInterface
{
public:


public:
    damiaoMotor(uint32_t id, bool inverse = false);
    virtual ~damiaoMotor();
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
    // #pragma pack(1)
    // typedef struct
    // {
    //     uint64_t t_ff_l : 8; //扭矩
    //     uint64_t t_ff_h : 4; //扭矩
    //     uint64_t Kd_l : 4;
    //     uint64_t Kd_h : 8;
    //     uint64_t Kp_l : 8;
    //     uint64_t Kp_h : 4;
    //     uint64_t v_des : 4;
    //     uint64_t v_des : 8;
    //     uint64_t p_des : 8;
    //     uint64_t p_des : 8;
    // } mitCanFram_t;
    // #pragma pack()
    // #pragma pack(1)
    // typedef struct
    // {
    //     uint64_t Temp_Rotor : 8; //线圈温度
    //     uint64_t Temp_MOS : 8;//MOS管温度
    //     uint64_t Torque : 8;
    //     uint64_t Torque : 4;
    //     uint64_t vel : 4;
    //     uint64_t vel : 8;
    //     uint64_t pos : 8;
    //     uint64_t pos : 8;
    //     uint64_t error : 4;
    //     uint64_t id : 4;
    // } feedbackCanFram_t;
    // #pragma pack()    

    struct motorInfo_t
    {
        float pos = 0.0;
        float vel = 0.0;
        float tor = 0.0;
        uint32_t error = 0;
    };

private:
    void MIT_CtrlMotor(uint16_t id, float _pos, float _vel,
    float _KP, float _KD, float _torq);
    void PosSpeed_CtrlMotor(uint16_t id, float _pos, float _vel);
    float uint_to_float(int x_int, float x_min, float x_max, int bits);
    int float_to_uint(float x, float x_min, float x_max, int bits);
    void recMsgCallback(CanBase::CanRxMsg msg);
private:
    CanBase *can_bus_ = nullptr;
    uint32_t timeout_ = 6; // s
    uint32_t enableTimeout = 3000; //ms
    bool isOnline_ = false;
    bool enableCmd = false;
    uint32_t modeID = 0;
    std::mutex mtx_;
    motorInfo_t motorInfo_;
    float maxVel = 45.0;
    std::chrono::time_point<std::chrono::steady_clock> last_time_;
    std::chrono::time_point<std::chrono::steady_clock> now_time_;
    std::chrono::time_point<std::chrono::steady_clock> enable_last_time_;
    std::chrono::time_point<std::chrono::steady_clock> enable_now_time_;    

};
