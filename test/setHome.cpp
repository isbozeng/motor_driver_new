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
#include <math.h>
int main(int agrc, char *argv[])
{

    nimotionMotor s1(1);
	// nimotionMotor s2(2);
    // feetechMotor m1(1);
    feetechMotor m2(2);
    // feetechMotor m3(3);  
    s1.init();
    // s2.init();
    // m1.init();
    m2.init();
    // m3.init();

    s1.setEnableStatus(false);
    // s2.setEnableStatus(true);
    // m1.setEnableStatus(true);    
    m2.setEnableStatus(false);
    // m3.setEnableStatus(true);




    bool enable = false;
    double curS1Pos = 0.0;
    double curM2Pos = 1.0;
    s1.setHome();
    m2.setHome();
    int32_t ret = -1;
	while (!(fabs(curS1Pos) < 0.01 && ret == 0))
	{
        ret = s1.getMotorPosition(curS1Pos);
        // s2.getMotorPosition(curS2Pos); 
        s1.getEnableStatus(enable);
        // s2.getEnableStatus(enable);
        std::cout<<"s1:"<<curS1Pos<<std::endl;
        usleep(500000);
    } 
	while (fabs(curM2Pos) > 0.3)
	{
        m2.getMotorPosition(curM2Pos);
        // s2.getMotorPosition(curS2Pos); 
        // s2.getEnableStatus(enable);
        std::cout<<"m2:"<<curM2Pos<<std::endl;    
        usleep(500000);
    } 
    std::cout<<"=========="<<std::endl;
    std::cout<<"s1:"<<curS1Pos<<std::endl;
    std::cout<<"m2:"<<curM2Pos<<std::endl;
}