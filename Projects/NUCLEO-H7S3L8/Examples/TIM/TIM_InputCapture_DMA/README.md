## <b>TIM_InputCapture_DMA Example Description</b>

This example demonstrates how to measure the LSI clock frequency thanks to the DMA interface of the TIM15 timer instance.

This project is targeted to run on STM32H7S3xx device on NUCLEO-H7S3L8 board from STMicroelectronics.
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.
This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

The project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.
The SystemCoreClockUpdate() function is used to update SystemCoreClock variable according to RCC registers values.
A non-cacheable RAM area is configured by the MPU_ConfigRAMBuffer() function.
The ClockConfig() function is called to activate the LSI clock and redirect it to the microcontroller clock output 2 (MCO2).

The TIM15 input clock (TIM15CLK) is set to APB2 clock (PCLK2).
SYSCLK = 600 MHz
CPU prescaler (CPRE) = DIV1
CPUCLK = SYSCLK/CPRE = 600 MHz
Bus Matrix prescaler (BMPRE) = DIV2
HCLK = CPUCLK/BMPRE = 275 MHz
APB2 prescaler (PPRE2) = DIV2
PCLK2 = HCLK/PPRE2 = 137.5 MHz
Timer prescaler (TIMPRE) = DIV1
Since PPRE2=DIV2 and TIMPRE=DIV1, APB2 timer clock is twice as fast as the APB2 peripheral clock (PCLK2).
APB2 timer clock = PCLK2 * 2 = 275 MHz

The MCO2 is internally connected to TIM15 input channel 1 thanks to TIM15 interconnection capabilities.
TIM15 channel 1 is started in input capture mode with DMA. From this point onward, a DMA request is generated
every time a rising edge is detected on the input channel 1 to transfer the captured value from the
capture 1 register to a buffer located in RAM. When the buffer is full (i.e., the DMA transfer is completed),
the minimum, maximum and average frequency values are computed and stored in uwMinFrequency, uwMaxFrequency,
uwAvgFrequency variables respectively.

LD1 is switched ON when the calculated average frequency value corresponds to the LSI frequency.
LD3 is switched ON in case of DMA transfer error or if the measured average frequency is out of range.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
    by the Cortex M7 and the GPDMA/HPDMA), there is no need for cache maintenance.
    If the application needs to put DMA buffers in AXI SRAM (starting from @0x24000000), the user has to:
    - either define a non-cacheable region in the MPU and linker configuration file to locate DMA buffers
      (a proposed noncacheable_buffer section is available from CMSIS Device linker template file and its size must
      be adapted to the application requirements)
    - or to ensure cache maintenance operations to ensure the cache coherence between the CPU and the DMAs.
    This is true also for any other data buffers accessed by the CPU and other masters (DMA2D, LTDC)
    The addresses and the size of cacheable buffers (shared between CPU and other masters)
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"


### <b>Keywords</b>

TIM, DMA, Input Capture, Frequency

### <b>Directory contents</b>
  - TIM/TIM_InputCapture_DMA/Appli/Inc/main.h                        Header for main.c module
  - TIM/TIM_InputCapture_DMA/Appli/Inc/stm32h7rsxx_hal_conf.h        HAL configuration file
  - TIM/TIM_InputCapture_DMA/Appli/Inc/stm32h7rsxx_it.h              Interrupt handlers header file
  - TIM/TIM_InputCapture_DMA/Appli/Inc/stm32h7rsxx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_InputCapture_DMA/Appli/Src/main.c                        Main program
  - TIM/TIM_InputCapture_DMA/Appli/Src/stm32h7rsxx_hal_msp.c         HAL MSP module
  - TIM/TIM_InputCapture_DMA/Appli/Src/stm32h7rsxx_it.c              Interrupt handlers
  - TIM/TIM_InputCapture_DMA/Appli/Src/system_stm32h7rsxx.c          STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3xx devices.

  - This example has been tested with an STMicroelectronics NUCLEO-H7S3L8
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

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