################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/driver.cpp \
../src/test_map.cpp \
../src/test_priority_queue.cpp 

OBJS += \
./src/driver.o \
./src/test_map.o \
./src/test_priority_queue.o 

CPP_DEPS += \
./src/driver.d \
./src/test_map.d \
./src/test_priority_queue.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"E:\Google Drive\Coding\eclip-c-workspace\courselib\src" -I"C:\Users\Young\workspace\googletestlib\include" -O0 -g3 -Wall -Wextra -Wconversion -c -fmessage-length=0 -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


