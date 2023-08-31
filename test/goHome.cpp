#include <stdio.h>
#include <iostream>
#include "controlcan.h"
#include <ctime>
#include <cstdlib>
#include "unistd.h"
#include "Nimotion.hpp"
#include "servocontrol.h"
#include <bitset>
#include <unistd.h>
#include <signal.h>
#include <time.h>

static const std::string GREEN_BOLD = "\033[1;32m";
static const std::string RED_BOLD = "\033[1;31m";
static const std::string RESET_FORMAT = "\033[0m";

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

    m1.SetAngleWithVelocityLimit(0, 5);
    m2.SetAngleWithVelocityLimit(0, 5);
    m3.SetAngleWithVelocityLimit(0, 5);
    m4260.SetAngleWithVelocityLimit(0, 5);
    m4248.SetAngleWithVelocityLimit(0, 5);
    sleep(1);    
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
    std::cout<<"----------"<<std::endl;
    std::cout<<"m1:"<<m1.angle<<std::endl;	
    std::cout<<"m2:"<<m2.angle<<std::endl;
    std::cout<<"m3:"<<m3.angle<<std::endl;
    std::cout<<"s1:"<<m4260.angle<<std::endl;
    std::cout<<"s2:"<<m4248.angle<<std::endl;		
    std::cout<<"----------"<<std::endl;


		usleep(100000);
	}   
    std::cout<<"----------"<<std::endl;
    std::cout<<"m1:"<<m1.angle<<std::endl;	
    std::cout<<"m2:"<<m2.angle<<std::endl;
    std::cout<<"m3:"<<m3.angle<<std::endl;
    std::cout<<"s1:"<<m4260.angle<<std::endl;
    std::cout<<"s2:"<<m4248.angle<<std::endl;		
    std::cout<<"----------"<<std::endl;
    return 0;
}