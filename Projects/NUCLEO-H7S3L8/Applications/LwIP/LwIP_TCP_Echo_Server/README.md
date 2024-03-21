
## <b>LwIP_TCP_Echo_Server application Description</b>

This application guides STM32Cube HAL API users to run TCP Echo Server application based on Raw API of LwIP TCP/IP stack.

####  <b>Expected success behavior</b>

 + The board IP address is printed on the HyperTerminal
 + The messages received by the Server are printed on the HyerTerminal
 + If the [echotool](https://github.com/PavelBansky/EchoTool/releases/tag/v1.5.0.0) utility is used the message sent by the Server are displayed on the PC console.

#### <b>Expected error behavior</b>

 + LED_YELLOW is ON indicating the ethernet cable is not connected
 + On failure, the Red Led is ON

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

LwIP, Ethernet, TCP/IP, DHCP, TCP echo Server

### <b>Hardware and Software environment</b>

  - This application runs on STM32H7S3L8 devices.

  - This application has been tested with the following environments:
     - NUCLEO-H7S3L8 revision B01 board
     - echotool: (http://bansky.net/echotool/) is used as echo server that sends
       back every incoming data.
     - DHCP server:  PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server

  - NUCLEO-H7S3L8 revision B01 Set-up
     - Connect the NUCLEO board to remote PC (through a crossover ethernet cable)
       or to your local network (through a straight ethernet cable)

  - Remote PC Set-up
     - PC must share the same LAN network configuration with the NUCLEO board
       the IP address must be the same as destination address value in (main.h)

  - This application uses USART3 to display logs, the hyperterminal configuration is as follows:
     - BaudRate = 115200 baud
     - Word Length = 8 Bits
     - Stop Bit = 1
     - Parity = None
     - Flow control = None

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

### <b>How to use it ?</b>

This application runs from the external Flash memory (LwIP_TCP_Echo_Server Appli).
It boots from internal Flash (Template_XIP_Boot) then jumps to the application code in external Flash.

To configure STM32CubeIDE Debug Configuration, you must do the following :

    1. Upload the template XIP
    2. Add the adequate external loader (MX25UW25645G_STM32H7R38-NUCLEO.stldr file) in Project->Debugger Configuration
    3. Add in the startup the template_XIP_Boot in Project->Debugger Configuration
    4. Move up the application in the startup

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Run the [echotool](https://github.com/PavelBansky/EchoTool/releases/tag/v1.5.0.0) utility on a windows console as following:

       C:\>echotool IP_address /p tcp /r 7 /n 15 /t 2 /d Testing LwIP TCP echo server

       Example : C:\>echotool 10.157.21.70 /p tcp /r 7 /n 15 /t 2 /d Testing LwIP TCP echo server

 - Rebuild all files and load your image into target memory
 - Run the application
