#ifndef _SMS_STS_THERAD_H
#define _SMS_STS_THREAD_H

#include "SMS_STS.h"
#include "stdint.h"
#include <thread>
#include <chrono>
#include <mutex>
#include <iostream>
#include <unordered_map>
#include <atomic>
// #define MAX_NUM 4 // 电机最大个数
// #define SERVO_ID_HASH(x) ((x) + 1)
namespace servo{

	struct servoStatus
    {
      // 读取信息
      float Pos_f;
      float Speed_f;
      int load; // 读输出至电机的电压百分比(0~1000)
      int vol;
      int temper;
      int isMoving; // 移动状态
      int current;
      bool isEnable;
      bool isOnline;
    };
	struct servoDriveInfo
	{
		int32_t id;
		int pos;
		int speed;
		int load; // 读输出至电机的电压百分比(0~1000)
		int vol;
		int temper;
		int isMoving; // 移动状态
		int current;
		bool isEnable;
		bool isOnline;

		int16_t posCmd;
		uint16_t velCmd;
		bool moveCmd;
		bool enableCmd;
		bool homeCmd;
		uint8_t accCmd;
		std::mutex mtx;
	};
	class SMS_STS_THREAD : public SMS_STS
	{
	public:
		SMS_STS_THREAD(); // 构造函数中启动线程
		virtual ~SMS_STS_THREAD();
		void getServoInfo(int32_t id, servoStatus &info);// 此状态为上层所用状态
		void addServo(int32_t id, servoDriveInfo *info);//此状态为驱动所用状态	

		std::unordered_map<int32_t, servoDriveInfo*>::iterator getItr(int32_t id)
		{
			// std::lock_guard<std::mutex> mylock(mapMtx);
			auto itr = servoInfo.end();
			if (mapMtx.try_lock())
			{
				itr = servoInfo.find(id);
				mapMtx.unlock();
			}
			return itr;
		}	
		void setPosVel(uint32_t id, int16_t angle, uint16_t vel)
		{
			// std::cout<<"angle:"<<(int)angle<<" vel:"<<(int)vel<<std::endl;
			auto itr = getItr(id);
			if (itr == servoInfo.end())
			{
				std::cout<<"itr null"<<std::endl;
				return;
			}			
			if (itr->second->mtx.try_lock())
			{
				itr->second->posCmd = angle;
				itr->second->velCmd = vel;
				// std::cout<<"angle:"<<(int)angle<<" vel:"<<(int)vel<<std::endl;
				if (!itr->second->moveCmd)
				{
					itr->second->moveCmd = true;
				}
				itr->second->mtx.unlock();
			}
		}
		void setEnable(int32_t id, bool _enable)
		{
			auto itr = getItr(id);
			if (itr == servoInfo.end())
			{
				return;
			}			
			std::lock_guard<std::mutex> mylock(itr->second->mtx);
			itr->second->enableCmd = _enable;
		}
		void SetAcceleration(int32_t id, uint8_t acc)
		{
			auto itr = getItr(id);
			if (itr == servoInfo.end())
			{
				return;
			}			
			std::lock_guard<std::mutex> mylock(itr->second->mtx);
			itr->second->accCmd = acc;
		}
		void ApplyPositionAsHome(int32_t id)
		{
			auto itr = getItr(id);
			if (itr == servoInfo.end())
			{
				return;
			}
			std::lock_guard<std::mutex> mylock(itr->second->mtx);
			itr->second->homeCmd = true;
		}
		static SMS_STS_THREAD *getInstance()
		{
			static SMS_STS_THREAD *instance = new SMS_STS_THREAD();
			return instance;
		}

	private:
		std::thread th;
		std::unordered_map<int32_t, servoDriveInfo*>servoInfo;
		std::mutex mapMtx;
		std::atomic<int32_t> servoCnt;
		void thread_f();	
	};

}


#endif