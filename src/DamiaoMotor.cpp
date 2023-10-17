#include "DamiaoMotor.h"

#define Motar_mode 0	//设置模式为何种模式，为0为IMT模式，为1为位置速度模式，为2为速度模式

#define P_MIN -12.5		//位置最小值
#define P_MAX 12.5		//位置最大值
#define V_MIN -45			//速度最小值
#define V_MAX 45			//速度最大值
#define KP_MIN 0.0		//Kp最小值
#define KP_MAX 500.0	//Kp最大值
#define KD_MIN 0.0		//Kd最小值
#define KD_MAX 5.0		//Kd最大值
#define T_MIN -18			//转矩最大值
#define T_MAX 18			//转矩最小值

#define GREEN_BOLD  "\033[1;32m"
#define RED_BOLD  "\033[1;31m"
#define RESET_FORMAT  "\033[0m"

damiaoMotor::damiaoMotor(uint32_t id, bool inverse)
    :CtrlMotorInterface(id, inverse)
    ,modeID(0x100)
{

}

damiaoMotor::~damiaoMotor()
{

}

int32_t damiaoMotor::init()
{
    can_bus_ = UsbCanBus::getCanBusInstance();
    if (can_bus_ != nullptr)
    {
        if (can_bus_->getOpenStatus() < 0)
        {
            return ERROR_CAN_DEVICE_FAIL;
        }
        can_bus_->CanReceiveRegister(nodeID_, &damiaoMotor::recMsgCallback, this);//modeID
        last_time_ = std::chrono::steady_clock::now();
        now_time_ = last_time_;
    }
    CanBase::CanTxMsg TxMsg;
    TxMsg.StdId = modeID|nodeID_;
    TxMsg.IDE = CanBase::CAN_ID_STD;
    TxMsg.RTR = CanBase::CAN_RTR_DATA;
    TxMsg.DLC = 0x08;
    TxMsg.Data[0] = 0xFF;
    TxMsg.Data[1] = 0xFF;
    TxMsg.Data[2] = 0xFF;
    TxMsg.Data[3] = 0xFF;
    TxMsg.Data[4] = 0xFF;
    TxMsg.Data[5] = 0xFF;
    TxMsg.Data[6] = 0xFF;  
    TxMsg.Data[7] = 0xFD;

    can_bus_->Transmit(TxMsg); 
    // can_bus_->Transmit(TxMsg); 
    sleep(2);
    return CMD_SUCCESS;
}

int32_t damiaoMotor::setMotorPosition(double pos)
{
    // MIT_CtrlMotor(modeID | nodeID_, pos, 0, float _KP, float _KD, 0);
    pos = inverse_direction_ ? -pos : pos;
    PosSpeed_CtrlMotor(modeID | nodeID_, pos, maxVel);
    return CMD_SUCCESS;
}

int32_t damiaoMotor::getMotorPosition(double& pos)
{
    pos = motorInfo_.pos;
    if(inverse_direction_)
    {
        pos = -pos;
    }
    return CMD_SUCCESS;
}

//非线程安全
int32_t damiaoMotor::setEnableStatus(bool enable)
{
    CanBase::CanTxMsg TxMsg;
    TxMsg.StdId = modeID | nodeID_;
    TxMsg.IDE = CanBase::CAN_ID_STD;
    TxMsg.RTR = CanBase::CAN_RTR_DATA;
    TxMsg.DLC = 0x08;
    TxMsg.Data[0] = 0xFF;
    TxMsg.Data[1] = 0xFF;
    TxMsg.Data[2] = 0xFF;
    TxMsg.Data[3] = 0xFF;
    TxMsg.Data[4] = 0xFF;
    TxMsg.Data[5] = 0xFF;
    TxMsg.Data[6] = 0xFF;    
    if (enable)
    {
        TxMsg.Data[7] = 0xFC;
    }
    else
    {
        TxMsg.Data[7] = 0xFD;
    }
	can_bus_->Transmit(TxMsg); 
    // can_bus_->Transmit(TxMsg); 
    enableCmd = enable;
    enable_last_time_ = std::chrono::steady_clock::now();
    return CMD_SUCCESS;
}

int32_t damiaoMotor::getEnableStatus(bool& enable)
{
        // 超时检测
    now_time_ = std::chrono::steady_clock::now();
    if (mtx_.try_lock()) {
        auto elapsedTime = now_time_ - last_time_;
        int32_t delt = std::chrono::duration_cast<std::chrono::seconds>(elapsedTime).count();
        if (delt > timeout_)
        {
            isOnline_ = false;
            std::cout << RED_BOLD << "nodeID_:" << (int)nodeID_ << " is offline!" << RESET_FORMAT << std::endl;
        }
        mtx_.unlock();
    }
    if (motorInfo_.error != 0)
    {
        switch(motorInfo_.error)
        {
            case 8: 
            {
                std::cout << RED_BOLD << "nodeID_:" << (int)nodeID_ << " 超压" << RESET_FORMAT << std::endl;
                // return ; //超压
            }break;
            case 9: 
            {
                std::cout << RED_BOLD << "nodeID_:" << (int)nodeID_ << " 欠压" << RESET_FORMAT << std::endl;
                // return ; //欠压
            }break;
            case 0xa: 
            {
                std::cout << RED_BOLD << "nodeID_:" << (int)nodeID_ << " 过流" << RESET_FORMAT << std::endl;
                // return ; //过流
            }break;
            case 0xb: 
            {
                std::cout << RED_BOLD << "nodeID_:" << (int)nodeID_ << " mos过温" << RESET_FORMAT << std::endl;
                // return ; //欠压
            }break;
            case 0xc:             
            {
                std::cout << RED_BOLD << "nodeID_:" << (int)nodeID_ << " 电机线圈过温" << RESET_FORMAT << std::endl;
                // return ; //欠压
            }break;
            case 0xd: 
            {
                std::cout << RED_BOLD << "nodeID_:" << (int)nodeID_ << " 通讯丢失" << RESET_FORMAT << std::endl;
                return ERROR_UNRECIVER_POS; //通讯丢失
            }break;
            case 0xe: 
            {
                std::cout << RED_BOLD << "nodeID_:" << (int)nodeID_ << " 过载" << RESET_FORMAT << std::endl;
                return ERROR_CURRENT_OVERLOAD; //过载
            }break;
        }
    }

    if (!isOnline_)
    {
        return ERROR_DEVICE_OFFLINE;
    }
    else if (enable != enableCmd)
    {
        enable_now_time_ = std::chrono::steady_clock::now();
        auto elapsedTime = enable_now_time_ - enable_last_time_;
        uint32_t delt = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count(); 
        if (delt > enableTimeout)
        {
            enable = enableCmd;
        }        
    }
    return CMD_SUCCESS;

}

int32_t damiaoMotor::setHome(uint32_t timeout)
{
    CanBase::CanTxMsg TxMsg;
    TxMsg.StdId = modeID | nodeID_;
    TxMsg.IDE = CanBase::CAN_ID_STD;
    TxMsg.RTR = CanBase::CAN_RTR_DATA;
    TxMsg.DLC=0x08;
    TxMsg.Data[0] = 0xFF;
    TxMsg.Data[1] = 0xFF;
    TxMsg.Data[2] = 0xFF;
    TxMsg.Data[3] = 0xFF;
    TxMsg.Data[4] = 0xFF;
    TxMsg.Data[5] = 0xFF;
    TxMsg.Data[6] = 0xFF; 
    TxMsg.Data[7] = 0xFE;

    auto setHomeLastTime = std::chrono::steady_clock::now();
    auto setHomeNowTime = std::chrono::steady_clock::now();
    auto elapsedTime = setHomeNowTime - setHomeLastTime;
    uint32_t delt = 0;

    while (delt < timeout)
    {

        can_bus_->Transmit(TxMsg); 
        usleep(10000);
        if (!isOnline_)
        {
            setEnableStatus(false);
        }
        else if ((motorInfo_.pos) > 1e-3)
        {
            break;
        }
        setHomeNowTime = std::chrono::steady_clock::now();
        elapsedTime = setHomeNowTime - setHomeLastTime;
        delt = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();         
    }
    if (delt >= timeout)
    {
        return ERROR_SET_HOME_TIMEOUT;
    }    
    return CMD_SUCCESS;
}

int32_t damiaoMotor::getTorque(double& torque)
{
    torque = motorInfo_.tor;
    return CMD_SUCCESS;
}

int32_t damiaoMotor::getVelocity(double& velocity)
{
    velocity = motorInfo_.vel;
    if(inverse_direction_)
    {
        velocity = -velocity;
    }    
    return CMD_SUCCESS;
}

void damiaoMotor::recMsgCallback(CanBase::CanRxMsg msg)
{
    int pos = 0, vel = 0, torque = 0;
    pos = (msg.Data[1] << 8) | msg.Data[2];
	vel = (msg.Data[3] << 4) | (msg.Data[4] >> 4);
	torque = ((msg.Data[4]&0xF) << 8) | msg.Data[5];

    std::lock_guard<std::mutex> lock(mtx_); // 锁定互斥锁    
	motorInfo_.pos = uint_to_float(pos, P_MIN, P_MAX, 16); 
	motorInfo_.vel = uint_to_float(vel, V_MIN, V_MAX, 12); 
	motorInfo_.tor = uint_to_float(torque, T_MIN, T_MAX, 12); 
    motorInfo_.error = msg.Data[0] & 0x0f;
    isOnline_ = true;
    last_time_ = std::chrono::steady_clock::now();
    if (motorInfo_.error > 7)
        std::cout << RED_BOLD << "nodeID_:" << (int)nodeID_ << " occur error:" <<motorInfo_.error << RESET_FORMAT << std::endl;
}

/**
 * @brief  采用浮点数据等比例转换成整数
 * @param  x_int     	要转换的无符号整数
 * @param  x_min      目标浮点数的最小值
 * @param  x_max    	目标浮点数的最大值
 * @param  bits      	无符号整数的位数
 */
float damiaoMotor::uint_to_float(int x_int, float x_min, float x_max, int bits){
/// converts unsigned int to float, given range and number of bits ///
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int)*span/((float)((1<<bits)-1)) + offset;
}

/**
 * @brief  将浮点数转换为无符号整数
 * @param  x     			要转换的浮点数
 * @param  x_min      浮点数的最小值
 * @param  x_max    	浮点数的最大值
 * @param  bits      	无符号整数的位数
 */

int damiaoMotor::float_to_uint(float x, float x_min, float x_max, int bits){
 /// Converts a float to an unsigned int, given range and number of bits///
    float span = x_max - x_min;
    float offset = x_min;
    return (int) ((x-offset)*((float)((1<<bits)-1))/span);
}


/**
 * @brief  MIT模式控下控制帧
 * @param  hcan   CAN的句柄
 * @param  ID     数据帧的ID
 * @param  _pos   位置给定
 * @param  _vel   速度给定
 * @param  _KP    位置比例系数
 * @param  _KD    位置微分系数
 * @param  _torq  转矩给定值
 */
void damiaoMotor::MIT_CtrlMotor(uint16_t id, float _pos, float _vel,
float _KP, float _KD, float _torq)
 { 
	CanBase::CanTxMsg    TxMsg;
	uint16_t pos_tmp,vel_tmp,kp_tmp,kd_tmp,tor_tmp;
	pos_tmp = float_to_uint(_pos, P_MIN, P_MAX, 16);
	vel_tmp = float_to_uint(_vel, V_MIN, V_MAX, 12);
	kp_tmp = float_to_uint(_KP, KP_MIN, KP_MAX, 12);
	kd_tmp = float_to_uint(_KD, KD_MIN, KD_MAX, 12);
	tor_tmp = float_to_uint(_torq, T_MIN, T_MAX, 12);

	TxMsg.StdId = id;
	TxMsg.IDE = CanBase::CAN_ID_STD;
	TxMsg.RTR = CanBase::CAN_RTR_DATA;
	TxMsg.DLC=0x08;
	
	TxMsg.Data[0] = (pos_tmp >> 8);
	TxMsg.Data[1] = pos_tmp;
	TxMsg.Data[2] = (vel_tmp >> 4);
	TxMsg.Data[3] = ((vel_tmp&0xF)<<4)|(kp_tmp>>8);
	TxMsg.Data[4] = kp_tmp;
	TxMsg.Data[5] = (kd_tmp >> 4);
	TxMsg.Data[6] = ((kd_tmp&0xF)<<4)|(tor_tmp>>8);
	TxMsg.Data[7] = tor_tmp;
	can_bus_->Transmit(TxMsg); 
 }

/**
 * @brief  位置速度模式控下控制帧
 * @param  hcan   CAN的句柄
 * @param  ID     数据帧的ID
 * @param  _pos   位置给定
 * @param  _vel   速度给定
 */
void damiaoMotor::PosSpeed_CtrlMotor(uint16_t id, float _pos, float _vel)
{
	CanBase::CanTxMsg    TxMsg;
    uint8_t *pbuf,*vbuf;
    pbuf=(uint8_t*)&_pos;
    vbuf=(uint8_t*)&_vel;

	TxMsg.StdId = id;
	TxMsg.IDE = CanBase::CAN_ID_STD;
	TxMsg.RTR = CanBase::CAN_RTR_DATA;
	TxMsg.DLC=0x08;

    TxMsg.Data[0] = *pbuf;;
    TxMsg.Data[1] = *(pbuf+1);
    TxMsg.Data[2] = *(pbuf+2);
    TxMsg.Data[3] = *(pbuf+3);
    TxMsg.Data[4] = *vbuf;
    TxMsg.Data[5] = *(vbuf+1);
    TxMsg.Data[6] = *(vbuf+2);
    TxMsg.Data[7] = *(vbuf+3);
	can_bus_->Transmit(TxMsg); 
    // can_bus_->Transmit(TxMsg); 
}