################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../diskget.cpp \
../diskinfo.cpp \
../disklist.cpp \
../diskput.cpp 

OBJS += \
./diskget.o \
./diskinfo.o \
./disklist.o \
./diskput.o 

CPP_DEPS += \
./diskget.d \
./diskinfo.d \
./disklist.d \
./diskput.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


