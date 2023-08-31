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
int main(int agrc, char *argv[])
{

	Nimotion m4260(1, false, 20, -180.0, 180.0, false);
	Nimotion m4248(2, false, 20, -180.0, 180.0, false);
	ServoMotion m1(1, false, 1, -180.0, 180.0);
	ServoMotion m2(2, false, 1, -180.0, 180.0);
	ServoMotion m3(3, false, 1, -180.0, 180.0); 
	while (m4260.state != Nimotion::FINISH 
	|| m4248.state != Nimotion::FINISH
	|| m1.state != Nimotion::FINISH
	|| m2.state != Nimotion::FINISH
	|| m3.state != Nimotion::FINISH)
	{
		m4260.UpdateAngle();
		m4248.UpdateAngle();
		m1.UpdateAngle();
		m2.UpdateAngle();
		m3.UpdateAngle();
		// std::cout<<"----"<<std::endl;
		// std::cout<<(int)m1.state<<std::endl;	
		// std::cout<<(int)m2.state<<std::endl;
		// std::cout<<(int)m3.state<<std::endl;
		// std::cout<<(int)m4260.state<<std::endl;
		// std::cout<<(int)m4248.state<<std::endl;		
		usleep(100000);
	}
    m1.SetEnable(false);
    m2.SetEnable(false);
    m3.SetEnable(false);
    m4260.SetEnable(false);
    m4248.SetEnable(false);
	while (m4260.state != Nimotion::STOP 
	|| m4248.state != Nimotion::STOP
    || m1.state != Nimotion::STOP
    || m2.state != Nimotion::STOP
    || m3.state != Nimotion::STOP)
	{
		m4260.UpdateAngle();
		m4248.UpdateAngle();
		m1.UpdateAngle();
		m2.UpdateAngle();
		m3.UpdateAngle();
        usleep(10000);
    }

    std::cout<<"----"<<std::endl;
    std::cout<<(int)m1.state<<std::endl;	
    std::cout<<(int)m2.state<<std::endl;
    std::cout<<(int)m3.state<<std::endl;
    std::cout<<(int)m4260.state<<std::endl;
    std::cout<<(int)m4248.state<<std::endl;	

}
