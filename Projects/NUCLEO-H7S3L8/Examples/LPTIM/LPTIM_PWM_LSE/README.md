## <b>LPTIM_PWM_LSE Example Description</b>

How to configure and use, through the HAL LPTIM API, the LPTIM peripheral using LSE 
as counter clock, to generate a PWM signal, in a low-power mode.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.

This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

In this example, the LPTIM instance used is LPTIM4 and the low power mode is Stop mode.

The counter clock is LSE (32.768 KHz), Autoreload equal to 99 so the output
frequency (FrequencyOutput) will be equal to 327.680.

    FrequencyOutput = Counter Clock Frequency / (Autoreload + 1)
                    = 32768 / 100
                    = 327.680 Hz

Pulse value equal to 49 and the duty cycle (DutyCycle) is computed as follow:

    DutyCycle = 1 - ((PulseValue + 1)/ (Autoreload + 1))
    DutyCycle = 50%

To minimize the power consumption, after starting generating the PWM signal,
the MCU enters in Stop mode. Note that GPIOs are configured in Low Speed to
enhance the consumption. 

USER push-button pin (PC.13)is configured as input with external interrupt (External line 13),
falling edge. When USER push-button is pressed, wakeup event is generated, PWM signal generation is stopped and LD1 is ON.

If LD3 is ON, an error is occurred.

#### <b>Notes</b>

 1. This example can not be used in DEBUG mode, this is due to the fact
    that the Cortex-M7 core is no longer clocked during low power mode
    so debugging features are disabled.

 2. Care must be taken when using HAL_Delay(), this function provides accurate
    delay (in milliseconds) based on variable incremented in SysTick ISR. This
    implies that if HAL_Delay() is called from a peripheral ISR process, then 
    the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 4. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
    by the Cortex M7 and the GPDMA/HPDMA), there is no need for cache maintenance.
    If the application needs to put DMA buffers in AXI SRAM (starting from @0x24000000), the user has to:
    - either define a non-cacheable region in the MPU and linker configuration file to locate DMA buffers
      (a proposed dma_buffer section is available from CMSIS Device linker template file and its size must
      be adapted to the application requirements)
    - or to ensure cache maintenance operations to ensure the cache coherence between the CPU and the DMAs.

    This is true also for any other data buffers accessed by the CPU and other masters (DMA2D, LTDC)
    The addresses and the size of cacheable buffers (shared between CPU and other masters)
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    - Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    - Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Timer, Low Power, PWM, Stop mode, Interrupt, LSE, Output, Duty Cycle

### <b>Directory contents</b>

  - LPTIM/LPTIM_PWM_LSE/Appli/Inc/stm32h7rsxx_nucleo_conf.h     BSP configuration file
  - LPTIM/LPTIM_PWM_LSE/Appli/Inc/stm32h7rsxx_hal_conf.h    HAL configuration file
  - LPTIM/LPTIM_PWM_LSE/Appli/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - LPTIM/LPTIM_PWM_LSE/Appli/Inc/main.h                  Header for main.c module  
  - LPTIM/LPTIM_PWM_LSE/Appli/Src/stm32h7rsxx_it.c          Interrupt handlers
  - LPTIM/LPTIM_PWM_LSE/Appli/Src/main.c                  Main program
  - LPTIM/LPTIM_PWM_LSE/Appli/Src/stm32h7rsxx_hal_msp.c     HAL MSP module
  - LPTIM/LPTIM_PWM_LSE/Appli/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled    

  - Connect PB5 (pin 14 in CN7 connector) to an oscilloscope to monitor the LPTIM4 output waveform.

  - LSE oscillator must be mounted on the board, else this example does not work.


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

**EWARM** and **MDK-ARM**:

 - Open your preferred toolchain
 - Rebuild all files from sub-project Appli and load your images into memories: This sub-project will first load Boot_XIP.hex in internal Flash,
   than load Appli part in External memory available on NUCLEO-H7S3L8 board.
 - Run the example

**CubeIDE**:

 - Compile the example/application; the elf file is required to configure the debug profile (the "active configuration" must be "debug", else only assembly debug is available)
 - Open the menu [Run]->[Debug configuration] and double click on  [STM32 C/C++ Application] (it creates a default debug configuration for the current project selected)
 - In [Debugger] tab, section "External  loaders" add the external loader corresponding to your Board/Memory as described below:
 - In "External loaders" section, click on [Add]
 - Select the loader among the available list (**MX25UW25645G_NUCLEO-H7S3L8.stldr** or **MX66UW1G45G_STM32H7S78-DK.stldr**)
 - Option "Enabled" checked and Option "Initialize" unchecked
 - In "Misc" section, uncheck the option "Verify flash download"
 - In [Startup] tab, section "Load Image and Symbols":
   - Click on [Add]
   - If your project contains a boot project:
     - click on "Project" and then select the boot project.
     - click on Build configuration and select "Use active".
     - then select the following options:
       - "Perform build" checked.
       - "Download" checked.
       - "Load symbols" unchecked.
   - If your project doesn't contain a boot project:
     - click on [File System] and select the Boot HEX file corresponding to your board

        Boot_XIP.hex can be found in folder [Binary] on each Template_XIP project

        You may need to force the capability to select a .hex file by typing " * " + pressing the "Enter" key in the file name dialog

     - then select the following options:
       - "Download"      checked.
       - "Load symbols" unchecked.
       - Click Ok
     - Back in the in the [Startup] tab, move down the boot project for it to be in second position
 - Our debug configuration is ready to be used.

