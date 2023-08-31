#ifndef CTRL_MOTOR_INTERFACE_H
#define CTRL_MOTOR_INTERFACE_H
#include <stdint.h>
#include "motor_error_code.h"
#include <math.h>
class CtrlMotorInterface
{
public:
    CtrlMotorInterface(uint32_t id, bool inverse = false) : nodeID_(id),inverse_direction_(inverse)
    {
    
    }
    virtual ~CtrlMotorInterface() = default;
    virtual int32_t init() = 0;//电机初始化
    virtual int32_t setMotorPosition(double pos) = 0;//设置电机位置角度,单位rad
    virtual int32_t getMotorPosition(double& pos) = 0;//读取当前电机位置角度，单位rad,电机出错反馈错误码
    virtual int32_t setEnableStatus(bool enable) = 0;//设置上下使能
    virtual int32_t getEnableStatus(bool& enable) = 0;//获得当前电机使能状态，在发送一条轨迹前需要先判断电机是否已经使能
    virtual int32_t setHome() = 0; //清0,将当前位置设置为零点

    /*以下几个是为了后期碰撞检测按读取力矩处理*/
    virtual int32_t getTorque(double& torque) = 0;//获得当前时刻电机力矩 单位:N*m
    virtual int32_t getVelocity(double& velocity) = 0;//获取当前时刻的速度(可选)  单位:rad/s
    
    uint32_t nodeID_;
    double angle_limit_max_;
    double angle_limit_min_;//单位，弧度
    bool inverse_direction_;//电机方向


};

#endif 
