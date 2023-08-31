all:
	g++ linearInterpolation.cpp  ../lib/*.so -I../inc -lpthread -std=c++11 -o line.elf
goHome:
	g++ goHome.cpp  src/*.cpp lib/*.so -Iinclude -lpthread -std=c++11 -o goHome.elf
disable:
	g++ disable.cpp  src/*.cpp lib/*.so -Iinclude -lpthread -std=c++11 -o disabel.elf
setHome:
	g++ setHome.cpp  src/*.cpp lib/*.so -Iinclude -lpthread -std=c++11 -o setHome.elf			
moveBystep:
	g++ moveByStep.cpp src/Nimotion.cpp src/UsbCanBus.cpp src/CanBase.cpp lib/libcontrolcan.so -Iinclude -lpthread -std=c++11 -o moveBystep.elf
atHome:
	g++ atHome.cpp src/Nimotion.cpp src/UsbCanBus.cpp src/CanBase.cpp lib/libcontrolcan.so -Iinclude -lpthread -std=c++11 -o atHome.elf
jointTest:
	g++ jointTest.cpp src/Nimotion.cpp src/UsbCanBus.cpp src/CanBase.cpp lib/libcontrolcan.so -Iinclude -lpthread -std=c++11 -o jointTest.elf
servoJointTest:
	g++ jointTestOfServo.cpp  src/servocontrol.cpp src/SMS_STS_THREAD.cpp lib/libSCServo.so -Iinclude  -lpthread -std=c++11 -o servoJointTest.elf
servoMoveByStep:
	g++ moveByStepOfServo.cpp src/servocontrol.cpp src/SMS_STS_THREAD.cpp lib/libSCServo.so -Iinclude -lpthread -std=c++11 -g -o  servoMoveByStep.elf
clean:
	rm -f *.o *.elf
