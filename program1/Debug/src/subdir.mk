################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/fa.cpp \
../src/ndfa.cpp \
../src/reachable.cpp \
../src/runoffvoting.cpp \
../src/wordgenerator.cpp 

OBJS += \
./src/fa.o \
./src/ndfa.o \
./src/reachable.o \
./src/runoffvoting.o \
./src/wordgenerator.o 

CPP_DEPS += \
./src/fa.d \
./src/ndfa.d \
./src/reachable.d \
./src/runoffvoting.d \
./src/wordgenerator.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"E:\Google Drive\Coding\eclip-c-workspace\courselib\src" -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


