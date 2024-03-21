## <b>TIM_OCActive Example Description</b>

Configuration of the TIM peripheral in Output Compare Active mode 
(when the counter matches the capture/compare register, the corresponding output 
pin is set to its active state).

This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The TIM1 frequency is set to SystemCoreClock/2, and the objective is
to get TIM1 counter clock at 10 kHz so the Prescaler is computed as following:

     - Prescaler = (TIM1CLK /TIM1 counter clock) - 1

    The TIM1 CCR1 register value is equal to 10000:
    TIM1_CH1 delay = CCR1_Val/TIM1 counter clock  = 1s
    so the TIM1 Channel 1 generates a signal with a delay equal to 1s.

    The TIM1 CCR2 register value is equal to 5000:
    TIM1_CH2 delay = CCR2_Val/TIM1 counter clock = 500 ms
    so the TIM1 Channel 2 generates a signal with a delay equal to 500 ms.

    The TIM1 CCR3 register value is equal to 2500:
    TIM1_CH3 delay = CCR3_Val/TIM1 counter clock = 250 ms
    so the TIM1 Channel 3 generates a signal with a delay equal to 250 ms.

    The TIM1 CCR4 register value is equal to 1250:
    TIM1_CH4 delay = CCR4_Val/TIM1 counter clock = 125 ms
    so the TIM1 Channel 4 generates a signal with a delay equal to 125 ms.

  The delay correspond to the time difference between PD10 rising edge and
  TIM1_CHx signal rising edges.

  STM32 board LED can be used to monitor the example status:

  - LD1 turns ON if example is OK.
  - LD3 toggles slowly in case of error.

#### <b>Notes</b>

 1. Delay values mentioned above are theoretical (obtained when the system clock frequency 
    is exactly 600 MHz). Since the generated system clock frequency may vary from one board to another observed
    delay might be slightly different.
  
 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
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

Timer, TIM, Output, Compare, Active, Signals, 

### <b>Directory contents</b>

  - TIM/TIM_OCActive/Inc/stm32h7rsxx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_OCActive/Appli/Inc/stm32h7rsxx_hal_conf.h  HAL configuration file
  - TIM/TIM_OCActive/Appli/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - TIM/TIM_OCActive/Appli/Inc/main.h                  Header for main.c module  
  - TIM/TIM_OCActive/Appli/Src/stm32h7rsxx_it.c        Interrupt handlers
  - TIM/TIM_OCActive/Appli/Src/main.c                  Main program
  - TIM/TIM_OCActive/Appli/Src/stm32h7rsxx_hal_msp.c   HAL MSP file
  - TIM/TIM_OCActive/Appli/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8 
    board and can be easily tailored to any other supported device 
    and development board.      

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up
   Connect the following pins to an oscilloscope to monitor the different waveforms:
      - Use LD1 connected to PD10 (Reference) (pin 65 in CN12 connector)
      - PE9: (TIM1_CH1) (pin 4 in CN10 connector)
      - PE11: (TIM1_CH2) (pin 6 in CN10 connector)
      - PE13: (TIM1_CH3) (pin 10 in CN10 connector)
      - PC12: (TIM1_CH4) (pin 10 in CN8 connector)

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

You should see these waveforms on oscilloscope :

 CH1                                                            ________________
 _______________________________________________________________|
           <---------------------- 1sec------------------------->
           
 CH2                                   __________________________________________
 ______________________________________|
           <------------500ms--------->
 
 CH3                        _____________________________________________________
 ___________________________|
           <----250ms------->
 
 CH4                  ____________________________________________________________
 _____________________|
           <--125ms--->

 LED       ______________________________________________________________________
 __________|


