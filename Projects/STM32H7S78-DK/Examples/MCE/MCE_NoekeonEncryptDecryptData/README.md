## <b>MCE_NoekeonEncryptDecryptData Example Description</b>

This project provides a description of how to encrypt and decrypt data from external memory (NOR).
The encryption/decryption of the data is performed on the fly using the MCE IP(Noekeon encryption algorithm).


At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32H7S7xx Devices.  

STM32H7S78-DK board's LED can be used to monitor the execution status:
 - [**LD1**]{style="color: GREEN"} toggles when the the encrypted data is successfully done
 - [**LD2**]{style="color: RED"}  toggles as soon as an error occurs. 

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

MCE, Cryptography, Security, Noekeon, Basics

### <b>Directory contents</b>

File | Description
 --- | ---
  MCE/MCE_NoekeonEncryptDecryptData/Boot/Inc/stm32h7rsxx_hal_conf.h        |  HAL configuration file  
  MCE/MCE_NoekeonEncryptDecryptData/Boot/Inc/stm32h7rsxx_it.h              |  Interrupt handlers header file
  MCE/MCE_NoekeonEncryptDecryptData/Boot/Inc/main.h                        |  Header for main.c module
  MCE/MCE_NoekeonEncryptDecryptData/Boot/Inc/stm32h7s78_discovery_conf.h   |  project discovery board config
  MCE/MCE_NoekeonEncryptDecryptData/Boot/Src/stm32h7rsxx_it.c              |  Interrupt handlers
  MCE/MCE_NoekeonEncryptDecryptData/Boot/Src/main.c                        |  Main program
  MCE/MCE_NoekeonEncryptDecryptData/Boot/Src/stm32h7rsxx_hal_msp.c         |  HAL MSP module
  MCE/MCE_NoekeonEncryptDecryptData/Boot/Src/system_stm32h7rsxx.c          |  STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S7xx devices.
  - In this example, the clock is set to 600 MHz.

  - This example has been tested with STMicroelectronics STM32H7S78-DK (MB1736)
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool):

    - XSPI1_HSLV=1 I/O XSPIM_P1 High speed option enabled
    - XSPI2_HSLV=1 I/O XSPIM_P2 High speed option enabled
    - VDDIO_HSLV=0 I/O speed optimized to operate at 3.3V

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

