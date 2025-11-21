##  <b>LwIP_HTTP_Server_Socket_RTOS Application Description</b>

This application guides STM32Cube HAL API users to run a http server application based on Socket API of LwIP TCP/IP stack.

- The communication is done with a web browser application in a remote PC.
- Since HTTPD_USE_CUSTOM_FSDATA flag is enabled, it requires to have in the generated project within "LwIP_HTTP_Server_Socket_RTOS \ Appli \ Inc \ " folder "fsdata_custom.c" file.
- If a DHCP server is available, a dynamic IP address can be allocated by enabling the DHCP process (#define LWIP_DHCP to 1 in lwipopts.h)
- At the beginning of the main program the HAL_Init() function is called to reset all the peripherals and initialize the Flash interface.
- The HAL initializes the TIM6 to generate an interrupt each 1ms, it will be used as time base for the HAL drivers.
- This application uses FreeRTOS, the RTOS initializes the systick to generate an interrupt each 1ms.
- The systick is then used for FreeRTOS time base.
- The CPU is at 600MHz

####  <b>Expected success behavior</b>

 + The board IP address is printed on the HyperTerminal
 + This HTTP server contains two html pages:
    - The first page (home page) is static, it gives information about STM32H7RS and LwIP stack.
    - The second page is dynamically refreshed (every 1 s), it shows the RTOS statistics in runtime.

#### <b>Expected error behavior</b>

   - LED_YELLOW is ON indicating the ethernet cable is not connected
   - LED_RED is toggling to indicate a critical error has occurred
   - The error message is printed on the HyperTerminal
   - The web server files are not properly loaded

#### <b>Assumptions if any</b>

- The Application is using the DHCP to acquire IP address, thus a DHCP server should be reachable by the board in the LAN used to test the application.

#### <b>Known limitations</b>

None.

### <b>Notes</b>

 1.  It is recommended to enable the cache and maintain its coherence:
      - Depending on the use case it is also possible to configure the cache attributes using the MPU.
      - Please refer to the **AN4838** "Managing memory protection unit (MPU) in STM32 MCUs".
      - Please refer to the **AN4839** "Level 1 cache on STM32F7 Series and STM32H7 Series".

### <b>Keywords</b>

Connectivity, LwIP, Ethernet, HTTP Server, Socket, TCP/IP, FreeRTOS, DHCP

### <b>Directory contents</b>

  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Inc/lwip.h                             header of lwip.c file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Inc/ethernetif.h                       header for ethernetif.c file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Inc/FreeRTOSConfig.h                   FreeRTOS configuration options
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Inc/stm32h7rsxx_hal_conf.h             HAL configuration file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Inc/stm32h7rsxx_it.h                   STM32 interrupt handlers header file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Inc/main.h                             Main program header file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Inc/lwipopts.h                         LwIP stack configuration options
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Inc/fsdata_custom.c                    ROM filesystem data (html pages)
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Src/lwip.c                             header of lwip.c file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Src/freertos.c                         Ethernet specific module
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Src/stm32h7rsxx_it.c                   STM32 interrupt handlers
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Src/main.c                             Main program
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Src/stm32h7rsxx_hal_timebase_tim.c     STM32H7RSxx system clock configuration file
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Src/ethernetif.c                       Interfacing LwIP to ETH driver
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Src/stm32h7rsxx_hal_msp.c              MSP Initialization
  - LwIP/LwIP_HTTP_Server_Socket_RTOS/Appli/Src/system_stm32h7rsxx.c               STM32H7RSxx system clock configuration file

### <b>Hardware and Software environment</b>

  - This application runs on STM32H7S3L8 devices.

  - This application has been tested with the following environments:
    - NUCLEO-H7S3L8 revision MB1737-H7S3L8-B02 board
    - Http client: Google Chrome
    - DHCP server: PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server

  - NUCLEO-H7S3L8 revision MB1737-H7S3L8-B02 Set-up
  - Connect the NUCLEO board to remote PC (through a crossover ethernet cable) or to your local network (through a straight ethernet cable)

  - This application uses USART3 to display logs, the hyperterminal configuration is as follows:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - Stop Bit = 1
    - Parity = None
    - Flow control = None

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

###  <b>How to use it ?</b>

This application runs from the external Flash memory (LwIP_HTTP_Server_Socket_RTOS Appli).
It boots from internal Flash (Template_XIP_Boot) then jumps to the application code in external Flash.

To configure STM32CubeIDE Debug Configuration, you must do the following :

    1. Upload the template XIP
    2. Add the adequate external loader (MX25UW25645G_NUCLEO-H7S3L8.stldr file) in Project->Debugger Configuration
    3. Add in the startup the template_XIP_Boot in Project->Debugger Configuration
    4. Move up the application in the startup

To run the program, follow these steps:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
