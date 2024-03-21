## <b>CORTEXM_MPU example Description</b>

Presentation of the MPU features. This example configures MPU attributes of different
MPU regions then configures a memory area as privileged read-only, and attempts to
perform read and write operations in different modes.

If the access is permitted LD1 is toggling. If the access is not permitted, 
a memory management fault is generated and LD3 is ON.

To generate an MPU memory fault exception due to an access right error, press
user button.

### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. For more details about the MPU configuration and use, please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"

### <b>Keywords</b>

System, Cortex, Memory regions, Privileged area, permitted access, memory management fault, MPU memory fault

### <b>Directory contents</b>

File | Description
 --- | ---
  CORTEX/CORTEXM_MPU/Boot/Inc/stm32h7rsxx_nucleo_conf.h    |  BSP configuration file
  CORTEX/CORTEXM_MPU/Boot/Inc/stm32h7rsxx_hal_conf.h       |  HAL configuration file
  CORTEX/CORTEXM_MPU/Boot/Inc/stm32h7rsxx_it.h             |  Interrupt handlers header file
  CORTEX/CORTEXM_MPU/Boot/Inc/main.h                       |  Header for main.c module
  CORTEX/CORTEXM_MPU/Boot/Src/stm32h7rsxx_it.c             |  Interrupt handlers
  CORTEX/CORTEXM_MPU/Boot/Src/stm32h7rsxx_hal_msp.c        |  HAL MSP file
  CORTEX/CORTEXM_MPU/Boot/Src/main.c                       | Main program
  CORTEX/CORTEXM_MPU/Boot/Src/system_stm32h7rsxx.c         |  stm32h7rsxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with a STM32H7S3L8Hx embedded on an
    NUCLEO-H7S3L8 board and can be easily tailored to any other supported
    device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

- Open your preferred toolchain
- Rebuild all files and load your image into target memory
- Run the example
