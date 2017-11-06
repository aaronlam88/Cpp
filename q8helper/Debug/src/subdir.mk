################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/driver_equivalence.cpp \
../src/empirical_equivalence.cpp 

OBJS += \
./src/driver_equivalence.o \
./src/empirical_equivalence.o 

CPP_DEPS += \
./src/driver_equivalence.d \
./src/empirical_equivalence.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"E:\Google Drive\Coding\eclip-c-workspace\courselib\src" -I"C:\Users\Young\workspace\googletestlib\include" -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


