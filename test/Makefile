all:
	g++ linearInterpolation.cpp  ../lib/*.so -I../inc -lpthread -std=c++11 -o line.elf
stepLine:
	g++ stepLinerInter.cpp  ../lib/*.so -I../inc -lpthread -std=c++11 -o stepLine.elf
goHome:
	g++ goHome.cpp  src/*.cpp lib/*.so -Iinclude -lpthread -std=c++11 -o goHome.elf
disable:
	g++ disable.cpp  ../lib/*.so -I../inc -lpthread -std=c++11 -o disable.elf
setHome:
	g++ setHome.cpp  ../lib/*.so -I../inc -lpthread -std=c++11 -o setHome.elf			
moveBystep:
	g++ moveByStep.cpp ../lib/*.so -I../inc -lpthread -std=c++11 -o moveBystep.elf
atHome:
	g++ atHome.cpp ../lib/*.so -I../inc -lpthread -std=c++11 -o atHome.elf
jointTest:
	g++ ../lib/*.so -I../inc -lpthread -std=c++11 -o jointTest.elf
servoJointTest:
	g++ ../lib/*.so -I../inc  -lpthread -std=c++11 -o servoJointTest.elf
servoMoveByStep:
	g++ moveByStepOfServo.cpp ../lib/*.so -I../inc -lpthread -std=c++11 -g -o  servoMoveByStep.elf
clean:
	rm -f *.o *.elf
