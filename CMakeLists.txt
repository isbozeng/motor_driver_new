cmake_minimum_required(VERSION 2.8.3) # 指定 CMake 最低版本要求

project(motor_driver) # 设置项目名称

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
#add_definitions(-DDEBUG) 
include_directories(${PROJECT_SOURCE_DIR}/inc)


file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

link_directories(${PROJECT_SOURCE_DIR}/lib)


add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
# 添加可执行文件，指定源文件
#add_executable(${PROJECT_NAME} moveByStepOfServo.cpp ${SOURCE_FILES})


target_link_libraries(${PROJECT_NAME} PRIVATE -lpthread -lcontrolcan -lSCServo) 

#set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_NAME}/lib)