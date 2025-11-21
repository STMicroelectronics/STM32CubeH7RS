
## <b>LwIP_HTTP_Server_Raw Application Description</b>

This application guides STM32Cube HAL API users to run a http server application based on Raw API of LwIP TCP/IP stack.
The communication is done with a web browser application in a remote PC.
Since HTTPD_USE_CUSTOM_FSDATA flag is enabled, it requires to have in the generated project within "LwIP_HTTP_Server_Raw \ Appli \ Inc \ " folder "fsdata_custom.c" file.

####  <b>Expected success behavior</b>

 + The board IP address is printed on the HyperTerminal
 + This HTTP server contains three html pages:
    - The first page (home page) is static, it gives information about STM32H7RS
    - The second page (LED control) allows to control the three LEDs located in
      the NUCLEO-H7S3L8 board
    - The third page (PB conversion) is dynamically refreshed (every 1 s),
      it shows the user push button input counter value

####  <b>Expected error behavior</b>

   - LED_YELLOW is ON indicating the ethernet cable is not connected
   - LED_RED is toggling to indicate a critical error has occurred
   - The error message is printed on the HyperTerminal
   - The web server files are not properly loaded
   - the user can't control the LEDs states or increment the PB counter value

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

Connectivity, LwIP, Ethernet, HTTP Server, Raw, TCP/IP, DHCP

### <b>Hardware and Software environment</b>

  - This application runs on STM32H7S3L8 devices.

  - This application has been tested with the following environments:
    - NUCLEO-H7S3L8 revision B02 board
    - Http client: Google Chrome
    - DHCP server:  PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server

  - NUCLEO-H7S3L8 revision B02 Set-up
     - Connect the NUCLEO board to remote PC (through a crossover ethernet cable)
       or to your local network (through a straight ethernet cable)

  - This application uses USART3 to display logs, the hyperterminal configuration is as follows:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - Stop Bit = 1
    - Parity = None
    - Flow control = None

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

### <b>How to use it ?</b>

This application runs from the external Flash memory (LwIP_HTTP_Server_Raw Appli).
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
