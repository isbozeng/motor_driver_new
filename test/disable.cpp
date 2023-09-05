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
    m2.setEnableStatus(false);
    bool enable = true;
    while(enable)
    {
        s1.getEnableStatus(enable);
        sleep(1);
    }
    std::cout<<"s1 enalbe:"<<std::boolalpha<<enable<<std::endl;
    while(enable)
    {
        m2.getEnableStatus(enable);
        sleep(1);
    }    
	std::cout<<"m2 enalbe:"<<std::boolalpha<<enable<<std::endl;
	return 0;
}
