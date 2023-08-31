#include "SMS_STS_THREAD.h"
#include "unistd.h"
#include <iostream>
#include <utility> // 导入utility头文件
static const std::string GREEN_BOLD = "\033[1;32m";
static const std::string RED_BOLD = "\033[1;31m";
static const std::string RESET_FORMAT = "\033[0m";
namespace servo
{
  SMS_STS_THREAD::SMS_STS_THREAD()
      : th(&SMS_STS_THREAD::thread_f, this), servoCnt(0)
  {
    begin(115200, "/dev/ttyUSB0");
    th.detach();
  }
  SMS_STS_THREAD::~SMS_STS_THREAD()
  {

  }


  void SMS_STS_THREAD::thread_f()
  {
    // std::cout<<"Thread state start."<<std::endl;
    uint32_t servoMoveCnt = 0;
    bool moveCmd = false;
    while (true)
    {
      mapMtx.lock();
      auto itr = servoInfo.begin();
      mapMtx.unlock();
      while (itr != servoInfo.end())
      {
        if (FeedBack(itr->first) != -1)
        {
          // std::cout<<"id:"<<itr->first<<std::endl;
          std::lock_guard<std::mutex> mylock(itr->second->mtx);
          itr->second->pos = ReadPos(-1);
          itr->second->speed = ReadSpeed(-1);
          itr->second->load = ReadLoad(-1);
          itr->second->vol = ReadVoltage(-1);
          itr->second->temper = ReadTemper(-1);
          itr->second->isMoving = ReadMove(-1);
          itr->second->current = ReadCurrent(-1);
          
          // itr->second->isOnline = true;
        }

        itr->second->mtx.lock();
        bool homeCmd = itr->second->homeCmd; // 互斥临时变量
        itr->second->mtx.unlock();

        if (homeCmd) // 清零
        {
          int ret = -1;
          ret = writeByte(itr->second->id, SMS_STS_TORQUE_ENABLE, 128);
          ////当前位置设置为2048，中间值,软件记为0，后面收到位置值做处理//写入，返回一个值，1或者0
          if (ret > 0)
          {
            std::lock_guard<std::mutex> mylock(itr->second->mtx);
            itr->second->homeCmd = false;
          }
        }
        else
        {
          int ret = readByte(itr->second->id, SMS_STS_TORQUE_ENABLE); // 读是否使能

          itr->second->mtx.lock();
          // std::cout<<"ret:"<<ret<<std::endl;
          if (ret != -1)
          {
            itr->second->isEnable = static_cast<bool>(ret);
            itr->second->isOnline = true; // 未读到，掉线
          }
          else
          {
            itr->second->isOnline = false;
            std::cout << RED_BOLD << "servo nodeid:" << (int)itr->second->id<< " read error!"<< RESET_FORMAT <<std::endl;
          }
          bool isOnline = itr->second->isOnline;
          bool enableCmd = itr->second->enableCmd; // 互斥临时变量
          bool isEnable = itr->second->isEnable;   // 互斥临时变量
          moveCmd = itr->second->moveCmd;
          int16_t posCmd = itr->second->posCmd;
          uint16_t velCmd = itr->second->velCmd;
          uint8_t acc = itr->second->accCmd;
          itr->second->mtx.unlock();

          if (enableCmd != isEnable)
          {
            writeByte(itr->second->id, SMS_STS_TORQUE_ENABLE, enableCmd);
          }
          // if (isOnline)
          // {
          if (moveCmd)
          {
            RegWritePosEx(itr->second->id, posCmd, velCmd, acc);
          }
            
          
          // servoMoveCnt++;
          // }
        }
        
        mapMtx.lock();
        itr++;
        mapMtx.unlock();
      }

      
      // if (moveCmd)
      // {
      //   std::cout<<"servoCnt:"<<servoCnt.load()<<std::endl;
        RegWriteAction();
      // }
      // servoMoveCnt = 0;
      usleep(5 * 1000);

    }

  }


void SMS_STS_THREAD::getServoInfo(int32_t id, servoStatus &info)
{
  
  auto itr = getItr(id);
  if (itr == servoInfo.end())
  {
    std::cout << RED_BOLD << "servo nodeid:" << (int)id << " not exit!\033[0m"<<std::endl;
    return;
  }
  
  if (itr->second->mtx.try_lock())
  {
    info.Pos_f = itr->second->pos * 0.088 - 180.0;
    info.Speed_f = itr->second->speed * (4.392 / 50);
    info.load = itr->second->load;
    info.vol = itr->second->vol;
    info.temper = itr->second->temper;
    info.isMoving = itr->second->isMoving;
    info.current = itr->second->current;
    info.isEnable = itr->second->isEnable;
    info.isOnline = itr->second->isOnline;
    itr->second->mtx.unlock();
  }
  // std::cout<<"id:"<<id<<" pos:"<<info.Pos_f<<" vel:"<<info.Speed_f<<std::endl;
}

void SMS_STS_THREAD::addServo(int32_t id, servoDriveInfo *info)
{
  mapMtx.lock();
  servoInfo[id] = info;
  mapMtx.unlock();
  servoCnt.fetch_add(1);
  std::cout<<"servo id:"<<id<<" init..."<<std::endl;
}
}