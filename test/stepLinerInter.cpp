#include <stdio.h>
#include <iostream>
#include "controlcan.h"
#include <ctime>
#include <cstdlib>
#include "unistd.h"
#include "NimotionMotor.h"
#include "FeetechMotor.h"
#include <bitset>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

static const std::string GREEN_BOLD = "\033[1;32m";
static const std::string RED_BOLD = "\033[1;31m";
static const std::string RESET_FORMAT = "\033[0m";


double linearInterpolation(double start, double end, double t0, double t1) 
{
    if (t0 <= 0) {
        return start;
    } else if (t0 >= t1) {
        return end;
    }

    // 计算线性插值结果
    double interpolatedValue = start + (end - start) * (t0 / t1);

    return interpolatedValue;
}

int main(int agrc, char *argv[])
{
    float tar[5][2] = {
        {30.0, 2000.0}, //m1
        {40.0, 2000.0}, //m2
        {30.0, 2000.0}, //m3
        {270.0, 5000.0}, //s1 guanjie2
        {-270.0, 5000.0}  //s2 guanjie3
    };
    nimotionMotor s1(1);
	nimotionMotor s2(2);
    // feetechMotor m1(1);
    // feetechMotor m2(2);
    // feetechMotor m3(3);  
    s1.init();
    s2.init();
    // m1.init();
    // m2.init();
    // m3.init();

    s1.setEnableStatus(true);
    s2.setEnableStatus(true);
    // m1.setEnableStatus(true);    
    // m2.setEnableStatus(true);
    // m3.setEnableStatus(true);



    uint32_t duration = 0;
    double s1Cmd = 0.0;
    double s2Cmd = 0.0;
    double m1Cmd = 0.0;
    double m2Cmd = 0.0;
    double m3Cmd = 0.0;

    double curS1Pos = 0.0;
    double curS2Pos = 0.0;
    double curM1Pos = 0.0;
    double curM2Pos = 0.0;
    double curM3Pos = 0.0;

    double curS1Vel = 0.0;
    double curS2Vel = 0.0;
    double curM1Vel = 0.0;
    double curM2Vel = 0.0;
    double curM3Vel = 0.0;    

    bool enable = false;
    while(!enable)
    {
        s1.getEnableStatus(enable);
        sleep(1);
    }

    while(!enable)
    {
        s2.getEnableStatus(enable);
        sleep(1);
    }

    // while(!enable)
    // {
    //     m1.getEnableStatus(enable);
    //     sleep(1);
    // }

    // while(!enable)
    // {
    //     m2.getEnableStatus(enable);
    //     sleep(1);
    // }

    // while(!enable)
    // {
    //     m3.getEnableStatus(enable);
    //     sleep(1);
    // }

    // m1.getMotorPosition(curM1Pos);
    // m2.getMotorPosition(curM2Pos);
    // m3.getMotorPosition(curM3Pos);
    s1.getMotorPosition(curS1Pos);
    s2.getMotorPosition(curS2Pos);


    auto curTime = std::chrono::system_clock::now();
    auto curTimeMs = std::chrono::time_point_cast<std::chrono::milliseconds>(curTime);  
    uint64_t curMs = curTimeMs.time_since_epoch().count(); 

	auto update_time = std::chrono::steady_clock::now();
	auto last_time = update_time;
	auto now_time = update_time;
    auto elapsedTime = now_time - last_time;

    std::cout<<"**********************"<<std::endl;
    std::cout<<"*angle ms:"<<curMs<<、*",m1:"<<curM1Pos<<",m2:"<<curM2Pos<<",m3:"<<curM3Pos*/
        <<",s1:"<<curS1Pos<<",s2:"<<curS2Pos<<std::endl;
    

    std::cout<<"*vel ms:"<<curMs/*<<",velM1:"<<curM1Vel<<",velM2:"<<curM2Vel<<",velM3:"<<curM3Vel*/
        <<",velS1:"<<curS1Vel<<",velS2:"<<curS2Vel<<std::endl;  
    std::cout<<"**********************"<<std::endl;
    // float m1Start = curM1Pos;
    // float m2Start = curM2Pos;
    // float m3Start = curM3Pos; 
    float s1Start = curS1Pos;
    float s2Start = curS2Pos;          
	while (true)
	{


        // m1Cmd = linearInterpolation(m1Start, m1Start + tar[0][0], duration, tar[0][1]);
        // m2Cmd = linearInterpolation(m2Start, m2Start + tar[1][0], duration, tar[1][1]);
        // m3Cmd = linearInterpolation(m3Start, m3Start + tar[2][0], duration, tar[2][1]);
        s1Cmd = linearInterpolation(s1Start, s1Start + tar[3][0], duration, tar[3][1]);
        s2Cmd = linearInterpolation(s2Start, s2Start + tar[4][0], duration, tar[4][1]);

        // m1.setMotorPosition(m1Cmd);
        // m2.setMotorPosition(m2Cmd);
        // m3.setMotorPosition(m3Cmd);
        s1.setMotorPosition(s1Cmd);
        s2.setMotorPosition(s2Cmd);            
		curTime = std::chrono::system_clock::now();
    	curTimeMs = std::chrono::time_point_cast<std::chrono::milliseconds>(curTime);
    	curMs = curTimeMs.time_since_epoch().count();
		std::cout<<"Cmdms:"<<curMs/*<<",m1Cmd:"<<m1Cmd<<",m2Cmd:"<<m2Cmd<<",m3Cmd:"<<m3Cmd*/
                <<",s1Cmd:"<<s1Cmd<<",s2Cmd:"<<s2Cmd<<",duration:"<<duration<<std::endl;

		auto updateElapsed = now_time - update_time;
		int32_t delt = std::chrono::duration_cast<std::chrono::milliseconds>(updateElapsed).count();
		if (delt > 40)//位置更新周期
        {
            // m1.getMotorPosition(curM1Pos);
            // m2.getMotorPosition(curM2Pos);
            // m3.getMotorPosition(curM3Pos);
            s1.getMotorPosition(curS1Pos);
            s2.getMotorPosition(curS2Pos); 
            s1.getEnableStatus(enable);
            s2.getEnableStatus(enable);
            // m1.getVelocity(curM1Vel);
            // m2.getVelocity(curM2Vel);
            // m3.getVelocity(curM3Vel);
            s1.getVelocity(curS1Vel);
            s2.getVelocity(curS2Vel);  

            curTime = std::chrono::system_clock::now();
            curTimeMs = std::chrono::time_point_cast<std::chrono::milliseconds>(curTime);
            curMs = curTimeMs.time_since_epoch().count();
            std::cout<<"Anglems:"<<curMs/*<<",m1:"<<curM1Pos<<",m2:"<<curM2Pos<<",m3:"<<curM3Pos*/
                    <<",s1:"<<curS1Pos<<",s2:"<<curS2Pos<<" ,duration:"<<duration<<std::endl;
            std::cout<<"vel ms:"<<curMs/*<<",velM1:"<<curM1Vel<<",velM2:"<<curM2Vel<<",velM3:"<<curM3Vel*/
                <<",velS1:"<<curS1Vel<<",velS2:"<<curS2Vel<<std::endl;   
            update_time = std::chrono::steady_clock::now();              
        }

		usleep(5000);
        now_time = std::chrono::steady_clock::now();
		elapsedTime = now_time - last_time;
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();        
	}
}
