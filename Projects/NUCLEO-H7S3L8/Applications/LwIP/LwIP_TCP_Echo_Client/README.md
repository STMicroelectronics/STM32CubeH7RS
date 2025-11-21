
## <b>LwIP_TCP_Echo_Client Application Description</b>

This application guides STM32Cube users to run TCP Echo Client application based on Raw API of LwIP TCP/IP stack.

####  <b>Expected success behavior</b>

 + The board IP address is printed on the STLINK USB serial console
 + Each time the user pushes the User button of the NUCLEO-H7S3L8 board, the messages sent by the client are printed on the serial console and an echo request is sent to the server
 + If the [echotool](https://github.com/PavelBansky/EchoTool/releases/tag/v1.5.0.0) utility is used, the message sent by the client are displayed on the PC console.

#### <b>Expected error behavior</b>

 + LED_YELLOW is ON indicating the ethernet cable is not connected

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

Connectivity, LwIP, Ethernet, TCP/IP, DHCP, TCP echo client

### <b>Hardware and Software environment</b>

  - This application runs on STM32H7S3L8 devices.

  - This application has been tested with the following environments:
    - NUCLEO-H7S3L8 revision B02 board
    - [echotool](http://bansky.net/echotool/) is used as echo server that sends
      back every incoming data.
    - DHCP server:  PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server

  - NUCLEO-H7S3L8 revision B02 Set-up
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

This application runs from the external Flash memory (LwIP_TCP_Echo_Client Appli).
It boots from internal Flash (Template_XIP_Boot) then jumps to the application code in external Flash.

To configure STM32CubeIDE Debug, you must do the following :

    1. Upload the template XIP
    2. Add the adequate external loader (MX25UW25645G_NUCLEO-H7S3L8.stldr file) in Project->Debugger Configuration
    3. Add in the startup the template_XIP_Boot in Project->Debugger Configuration
    4. Move up the application in the startup

To run the program, follow these steps:

 - Open your preferred toolchain
 - Run the [echotool](https://github.com/PavelBansky/EchoTool/releases/tag/v1.5.0.0) utility on a Windows PC command prompt:

       c:\> .\echotool.exe /p tcp /s

 - Update the echo server IP address and port in source file Appli/Src/lwip.c (#define DEST_IP_ADDR0, DEST_IP_ADDR1, ...)
 - Rebuild all files 
 - load the image into STM32 memory and run the application
 - press the board "User" blue button to send echo packet to the server
 - check on Echo server tool that it receives the message
