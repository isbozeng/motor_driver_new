#include "FeetechMotor.h"

#include <iostream>

feetechMotor::feetechMotor(uint32_t id, bool inverse)
    :CtrlMotorInterface(id, inverse)
{

}

feetechMotor::~feetechMotor()
{

}

int32_t feetechMotor::init()
{
    memset(&servoDrv, 0, sizeof(servo::servoDriveInfo));
    memset(&servoInf, 0, sizeof(servo::servoStatus));
    servoDrv.id = nodeID_;
    servoDrv.enableCmd = false;
    sm_st = servo::SMS_STS_THREAD::getInstance();
    sm_st->addServo(nodeID_, &servoDrv);
    return CMD_SUCCESS;
}

int32_t feetechMotor::setMotorPosition(double pos)
{
  uint16_t Angle_ref = (u16)((pos + 180) * (4096.0 / 360.0));
  uint16_t Vel_ref = 0;
  int32_t ret = CMD_SUCCESS;
  if (servoDrv.mtx.try_lock())
  {
    if (servoDrv.isOnline)
    {
        servoDrv.posCmd = Angle_ref;
        servoDrv.velCmd = Vel_ref;
        servoDrv.accCmd = 250; 
        if (!servoDrv.moveCmd)
        {
        servoDrv.moveCmd = true;
        }
    }
    else
    {
        ret = ERROR_SERIAL_FAIL;
    }
    servoDrv.mtx.unlock();
  }  
  return ret;
}

int32_t feetechMotor::getMotorPosition(double& pos)
{
    std::lock_guard<std::mutex> mylock(servoDrv.mtx);
    pos = servoDrv.pos * 0.088 - 180.0;
    return CMD_SUCCESS;
}

int32_t feetechMotor::setEnableStatus(bool enable)
{
    std::lock_guard<std::mutex> mylock(servoDrv.mtx);
    servoDrv.enableCmd = true;
    return CMD_SUCCESS;
}

int32_t feetechMotor::getEnableStatus(bool& enable)
{
    std::lock_guard<std::mutex> mylock(servoDrv.mtx);
    enable = servoDrv.isEnable;
    return CMD_SUCCESS;
}

int32_t feetechMotor::setHome()
{
    std::lock_guard<std::mutex> mylock(servoDrv.mtx);
    servoDrv.homeCmd = true;
    return CMD_SUCCESS;
}

int32_t feetechMotor::getTorque(double& torque)
{
    std::lock_guard<std::mutex> mylock(servoDrv.mtx);
    torque = servoDrv.current * 0.001;
    return CMD_SUCCESS;
}

int32_t feetechMotor::getVelocity(double& velocity)
{
    std::lock_guard<std::mutex> mylock(servoDrv.mtx);
    velocity = servoDrv.speed * (4.392 / 50);
    return CMD_SUCCESS;   
}

void feetechMotor::getState()
{
  if (servoDrv.mtx.try_lock())
  {
    servoInf.Pos_f = servoDrv.pos * 0.088 - 180.0;
    servoInf.Speed_f = servoDrv.speed * (4.392 / 50);
    servoInf.load = servoDrv.load;
    servoInf.vol = servoDrv.vol;
    servoInf.temper = servoDrv.temper;
    servoInf.isMoving = servoDrv.isMoving;
    servoInf.current = servoDrv.current;
    servoInf.isEnable = servoDrv.isEnable;
    servoInf.isOnline = servoDrv.isOnline;
    servoDrv.mtx.unlock();
  }    
}