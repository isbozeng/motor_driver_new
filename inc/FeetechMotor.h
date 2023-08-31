#pragma once
#include <unistd.h>
#include <stdint.h>
#include <string>
#include "ctrl_motor_interface.h"
#include "SMS_STS_THREAD.h"

class feetechMotor : public CtrlMotorInterface
{
public:
    feetechMotor(uint32_t id, bool inverse = false);
    virtual ~feetechMotor();
    virtual int32_t init() override;//电机初始化
    virtual int32_t setMotorPosition(double pos) override;//设置电机位置角度,单位rad
    virtual int32_t getMotorPosition(double& pos) override;//读取当前电机位置角度，单位rad,电机出错反馈错误码
    virtual int32_t setEnableStatus(bool enable) override;//设置上下使能
    virtual int32_t getEnableStatus(bool& enable) override;//获得当前电机使能状态，在发送一条轨迹前需要先判断电机是否已经使能
    virtual int32_t setHome() override; //清0,将当前位置设置为零点

    /*以下几个是为了后期碰撞检测按读取力矩处理*/
    virtual int32_t getTorque(double& torque) override;//获得当前时刻电机力矩 单位:N*m
    virtual int32_t getVelocity(double& velocity) override;//获取当前时刻的速度(可选)  单位:rad/s

private:
    servo::SMS_STS_THREAD *sm_st = nullptr;
    servo::servoStatus servoInf; // 保存电机状态参数
    servo::servoDriveInfo servoDrv;//电机驱动数据
    float cur_vel = 0.0;
    
    void getState();

};