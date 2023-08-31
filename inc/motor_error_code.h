#ifndef _MOTOR_ERROR_CODE_H_
#define _MOTOR_ERROR_CODE_H_

#define CMD_SUCCESS 0  //代表命令执行成功

#define ERROR_MOTOR_INIT 1001   //电机初始化错误
#define ERROR_REBOOT 1002   //

#define ERROR_WRITE_ENABLE 2001   //设置电机使能错误
#define ERROR_WRITE_MOTOR_POS 2002   //设置电机位置错误
#define ERROR_WRITE_MOTOR_CURRENT 2003   //设置电机电流错误
#define ERROR_WRITE_MOTOR_TORQUE 2004   //设置电机力矩错误

#define ERROR_READ_MOTOR_POS 3001   //读取电机位置错误  
#define ERROR_READ_MOTOR_CURRENT 3002   //读取电机电流错误  
#define ERROR_READ_MOTOR_TORQUE 3003   //读取电机扭矩错误
#define ERROR_READ_MOTOR_VELOCITY 3004   //读取电机速度错误
#define ERROR_READ_MOTOR_ENABLE_STATUS 3005   //读取电机使能状态错误

#define ERROR_CURRENT_OVERLOAD 4001   //电流过载错误


#define ERROR_CAN_DEVICE_FAIL 5001 //CAN 打开失败
#define ERROR_SERIAL_FAIL 5002 //串口错误

#endif
