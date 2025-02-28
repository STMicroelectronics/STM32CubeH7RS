## <b>LwIP_TFTP_Server application Description</b>

This application guides STM32Cube HAL API users to run a tftp server application for STM32H7S7xx devices.

- User has to use a PC SW tool (ex. TFTPD32) to send/receive data to/from micro SD card installed in STM32H7S78-DK board.
- If a DHCP server is available, a dynamic IP address can be allocated by enabling the DHCP process (#define LWIP_DHCP in lwipopts.h)
- The maximum length of filename used to send or to receive to/from uSD is set to 8 bytes and 3 bytes for file type
- The maximum transfer block size is set 512 Bytes
- Please insure that the uSD is plugged before starting, this application doesn't handle hot plug/unplug of the uSD card
- At the beginning of the main program, the HAL_Init() function is called to reset all the peripherals and initialize the systick used as 1ms HAL timebase.
- The SystemCoreClockUpdate() function is used to configure the system clock (SYSCLK) to run at 600 Mhz.
- The Application project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable the Layer 1 Core Instruction and Data Caches.
- This is provided as template implementation that the User may integrate in his application in order to enhance the performance.
- The CPU is at 600MHz.

####  <b>Expected success behavior</b>

   - LED_BLUE is ON indicating the board IP address has been assigned
   - The board IP address is printed on the HyperTerminal
   - The user can send/receive data to/from micro SD card using TFTPD32 SW tool

#### <b>Expected error behavior</b>

   - LED_ORANGE is ON indicating the ethernet cable is not connected
   - LED_RED is toggling to indicate a critical error has occurred
   - The error message is printed on the HyperTerminal
   - Timeout message is displayed on TFTPD32 SW tool

#### <b>Assumptions if any</b>

- The Application is using the DHCP to acquire IP address, thus a DHCP server should be reachable by the board in the LAN used to test the application.

#### <b>Known limitations</b>

- The filename used to send or receive to/from uSD should not contain any special characters and its maximum length is set to 30 characters.

### <b>Notes</b>

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
    - or to ensure cache maintenance operations to ensure the cache coherence between the CPU and the DMAs.
    This is true also for any other data buffers accessed by the CPU and other masters (DMA2D, LTDC)
    The addresses and the size of cacheable buffers (shared between CPU and other masters)
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.

Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Connectivity, LwIP, XIP, Ethernet, TFTP Server, TCP/IP, DHCP, send, receive, SD Card

### <b>Directory contents</b>

#### <b>Sub-project Appli</b>

    - LwIP/LwIP_TFTP_Server/Appli/Inc/ethernetif.h                 header for ethernetif.c file
    - LwIP/LwIP_TFTP_Server/Appli/Inc/ffconf.h                     FAT file system module configuration file
    - LwIP/LwIP_TFTP_Server/Appli/Inc/lwip.h                       header for lwip.c file
    - LwIP/LwIP_TFTP_Server/Appli/Inc/fatfs.h                      header for fatfs.c file
    - LwIP/LwIP_TFTP_Server/Appli/Inc/lwipopts.h                   LwIP stack configuration options
    - LwIP/LwIP_TFTP_Server/Appli/Inc/main.h                       Main program header file
    - LwIP/LwIP_TFTP_Server/Appli/Inc/sd_diskio_config.h           SD Disk IO header file
    - LwIP/LwIP_TFTP_Server/Appli/Inc/stm32h7rsxx_it.h             header for stm32h7rsxx_it.c
    - LwIP/LwIP_TFTP_Server/Appli/Inc/stm32h7rsxx_hal_conf.h       HAL Configuration file
    - LwIP/LwIP_TFTP_Server/Appli/Inc/ethernetif.c                 Interfacing LwIP to ETH driver
    - LwIP/LwIP_TFTP_Server/Appli/Inc/fatfs.c                      FAT file system module configuration file
    - LwIP/LwIP_TFTP_Server/Appli/Inc/lwip.c                       header for lwip.c file
    - LwIP/LwIP_TFTP_Server/Appli/Inc/stm32h7rsxx_hal_msp.c        HAL MSP module
    - LwIP/LwIP_TFTP_Server/Appli/Inc/main.c                       Main program
    - LwIP/LwIP_TFTP_Server/Appli/Inc/stm32h7rsxx_it.h             header for stm32h7rsxx_it.c
    - LwIP/LwIP_TFTP_Server/Appli/Inc/stm32h7rsxx_hal_conf.h       HAL Configuration file

### <b>Hardware and Software environment</b>

  - This application runs on STM32H7S7xx devices.

  - This application has been tested with the following environments:
    - STM32H7S78-DK MB1736 Rev. D01 board
    - DHCP server:  PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server

  - STM32H7S78-DK MB1736 Rev. D01 Set-up
  - Connect the board to remote PC (through a crossover ethernet cable) or to your local network (through a straight ethernet cable)

  - This application uses UART4 to display logs, the hyperterminal configuration is as follows:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - Stop Bit = 1
    - Parity = None
    - Flow control = None

  - On STM32H7S78-DK board, the BOOT0 mechanical slide switch must be set to SW1.
  - On STM32H7S78-DK board, the JP6 pin [2-3] must be ON.

  - The following User Option Bytes are set in this template:

    - XSPI1_HSLV=1     I/O XSPIM_P1 High speed option enabled
    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled
    - VDDIO_HSLV=1     I/O speed optimization enabled

### <b>How to use it ?</b>

This application runs from the external Flash memory (LwIP_TFTP_Server Appli).
It boots from internal Flash (Template_XIP_Boot) then jumps to the application code in external Flash.

To configure STM32CubeIDE Debug Configuration, you must do the following :

    1. Upload the template XIP
    2. Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration
    3. Add in the startup the template_XIP_Boot in Project->Debugger Configuration
    4. Move up the application in the startup

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Use PC utility TFTPD32 to perform file transfers, refer to UM1713 for details about this tool usage.

