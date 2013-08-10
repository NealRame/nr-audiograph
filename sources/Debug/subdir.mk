################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Parser.cpp \
../WaveFormGenerator.cpp \
../main.cpp 

OBJS += \
./Parser.o \
./WaveFormGenerator.o \
./main.o 

CPP_DEPS += \
./Parser.d \
./WaveFormGenerator.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DDEBUG -I"/home/jux/Documents/development/NRAudio" -I"/home/jux/Documents/development/NRGraph" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


