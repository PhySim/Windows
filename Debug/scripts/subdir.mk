################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../scripts/scene1.cpp 

OBJS += \
./scripts/scene1.o 

CPP_DEPS += \
./scripts/scene1.d 


# Each subdirectory must supply rules for building sources it contributes
scripts/%.o: ../scripts/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"D:\Google Drive\Portable Apps Platform\PortableApps\EclipsePortable\Data\workspace\PhySim" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


