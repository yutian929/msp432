################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"D:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/EGR424workspace/MPU6050_msp432/msp432Peripherals" --include_path="D:/EGR424workspace/MPU6050_msp432/msp432Peripherals/ESP" --include_path="D:/EGR424workspace/MPU6050_msp432/msp432Peripherals/mpuUart" --include_path="D:/EGR424workspace/MPU6050_msp432/eMPL-hal" --include_path="D:/EGR424workspace/MPU6050_msp432" --include_path="D:/EGR424workspace/MPU6050_msp432/mllite" --include_path="D:/EGR424workspace/MPU6050_msp432/mpl" --include_path="D:/EGR424workspace/MPU6050_msp432/drivers/eMPL" --include_path="D:/EGR424workspace/MPU6050_msp432/drivers/include" --include_path="D:/ti/ccsv7/ccs_base/arm/include/" --include_path="D:/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="C:/ti/msp/MSP432Ware_3_50_00_02/driverlib/driverlib/MSP432P4xx" --include_path="D:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include/" --include_path="D:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=MPU6050 --define=MOTION_DRIVER_TARGET_MSP432 --define=TARGET_IS_MSP432P4XX --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="main.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup_msp432p401r_ccs.obj: ../startup_msp432p401r_ccs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"D:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/EGR424workspace/MPU6050_msp432/msp432Peripherals" --include_path="D:/EGR424workspace/MPU6050_msp432/msp432Peripherals/ESP" --include_path="D:/EGR424workspace/MPU6050_msp432/msp432Peripherals/mpuUart" --include_path="D:/EGR424workspace/MPU6050_msp432/eMPL-hal" --include_path="D:/EGR424workspace/MPU6050_msp432" --include_path="D:/EGR424workspace/MPU6050_msp432/mllite" --include_path="D:/EGR424workspace/MPU6050_msp432/mpl" --include_path="D:/EGR424workspace/MPU6050_msp432/drivers/eMPL" --include_path="D:/EGR424workspace/MPU6050_msp432/drivers/include" --include_path="D:/ti/ccsv7/ccs_base/arm/include/" --include_path="D:/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="C:/ti/msp/MSP432Ware_3_50_00_02/driverlib/driverlib/MSP432P4xx" --include_path="D:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include/" --include_path="D:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=MPU6050 --define=MOTION_DRIVER_TARGET_MSP432 --define=TARGET_IS_MSP432P4XX --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="startup_msp432p401r_ccs.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

system_msp432p401r.obj: ../system_msp432p401r.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"D:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/EGR424workspace/MPU6050_msp432/msp432Peripherals" --include_path="D:/EGR424workspace/MPU6050_msp432/msp432Peripherals/ESP" --include_path="D:/EGR424workspace/MPU6050_msp432/msp432Peripherals/mpuUart" --include_path="D:/EGR424workspace/MPU6050_msp432/eMPL-hal" --include_path="D:/EGR424workspace/MPU6050_msp432" --include_path="D:/EGR424workspace/MPU6050_msp432/mllite" --include_path="D:/EGR424workspace/MPU6050_msp432/mpl" --include_path="D:/EGR424workspace/MPU6050_msp432/drivers/eMPL" --include_path="D:/EGR424workspace/MPU6050_msp432/drivers/include" --include_path="D:/ti/ccsv7/ccs_base/arm/include/" --include_path="D:/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="C:/ti/msp/MSP432Ware_3_50_00_02/driverlib/driverlib/MSP432P4xx" --include_path="D:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include/" --include_path="D:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=MPU6050 --define=MOTION_DRIVER_TARGET_MSP432 --define=TARGET_IS_MSP432P4XX --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="system_msp432p401r.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


