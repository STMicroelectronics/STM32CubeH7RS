## <b>OpenBootloader Application Description</b>

This application exploits OpenBootloader Middleware to demonstrate how to develop an IAP application
and how use it.

This application supports two workspaces, one runs from FLASH and one runs RAM memory:

- Workspace for user FLASH: compiled application ROM code is programmed in user FLASH and read write data are programmed in SRAM
- Workspace for user SRAM: compiled application ROM code and read write data are programmed in SRAM

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 200 Mhz.

Then, these protocols and memories interfaces are configured:

  - USART4
  - FDCAN2
  - I2C1
  - SPI4
  - USB
  - I3C1
  - FLASH interface
  - RAM interface
  - Option bytes interface
  - Engi bytes interface
  - OTP interface
  - System memory interface
  - flash read only interface
  - External memory interface

Then, the OpenBootloader application will wait for incoming communication on one of the supported protocols.

Once a communication is established through one of the supported protocols,
the OpenBootloader application will wait for commands sent by the host.

#### <b>Notes</b>

1. The current implementation of write in option bytes doesn’t support writing in OEMKEYR option bytes. The OEMKEYR option bytes registers programming can be easily added at the end of the function `OPENBL_OB_Write` in the file `optionbytes_interface.c`.

2. In case of Mass Erase operation, the OpenBootloader FLASH area must be protected otherwise the OpenBootloader will be erased.

3. In the `OpenBootloader_Init()` function in `app_openbootloader.c` file, the user can:
       - Select the list of supported commands for a specific interface by defining its own list of commands.

       Here is an example of how to customize USART interface commands list, here only read/write commands are supported:

        OPENBL_CommandsTypeDef USART_Cmd =
        {
          NULL,
          NULL,
          NULL,
          OPENBL_USART_ReadMemory,
          OPENBL_USART_WriteMemory,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL
        };

        USART_Handle.p_Ops = &USART_Ops;
        USART_Handle.p_Cmd = &USART_Cmd;           /* Initialize the USART handle with the list of supported commands */
        OPENBL_USART_SetCommandsList(&USART_Cmd);  /* Register the list of supported commands in MW side */

       - Use the default list of supported commands for a specific interface by reusing the commands list defined in MW side.

       Here is an example of how to use USART interface default commands list:

        /* Register USART interfaces */
        USART_Handle.p_Ops = &USART_Ops;
        USART_Handle.p_Cmd = OPENBL_USART_GetCommandsList();  /* Initialize the USART handle with the default list supported commands */

### <b>Keywords</b>

OpenBootloader, USART, FDCAN, I2C, I3C, SPI, USB

### <b>Directory contents</b>

     - Core/Src/main.c                                    Main program file
     - Core/Src/stm32h7rsxx_hal_msp.c                     MSP Initialization file
     - Core/Src/stm32h7rsxx_it.c                          Interrupt handlers file
     - Core/Src/system_stm32h7rsxx.c                      STM32U5xx system clock configuration file
     - Core/Inc/main.h                                    Main program header file
     - Core/Inc/stm32h7rsxx_hal_conf.h                    HAL Library Configuration file
     - Core/Inc/stm32h7rsxx_it.h                          Interrupt handlers header file
     - OpenBootloader/App/app_openbootloader.c            OpenBootloader application entry point
     - OpenBootloader/App/app_openbootloader.h            Header for OpenBootloader application entry file
     - OpenBootloader/Target/common_interface.c           Contains common functions used by different interfaces
     - OpenBootloader/Target/common_interface.h           Header for common functions file
     - OpenBootloader/Target/engibytes_interface.c        Contains ENGI Bytes interface
     - OpenBootloader/Target/engibytes_interface.h        Header of ENGI Bytes interface file
     - OpenBootloader/Target/external_memory_interface.c  Contains External memory interface
     - OpenBootloader/Target/external_memory_interface.h  Header of External memory interface file
     - OpenBootloader/Target/fdcan_interface.c            Contains FDCAN interface
     - OpenBootloader/Target/fdcan_interface.h            Header of FDCAN interface file
     - OpenBootloader/Target/flash_interface.c            Contains FLASH interface
     - OpenBootloader/Target/flash_interface.h            Header of FLASH interface file
     - OpenBootloader/Target/i2c_interface.c              Contains I2C interface
     - OpenBootloader/Target/i2c_interface.h              Header of I2C interface file
     - OpenBootloader/Target/i3c_interface.c              Contains I3C interface
     - OpenBootloader/Target/i3c_interface.h              Header of I3C interface file
     - OpenBootloader/Target/spi_interface.c              Contains SPI interface
     - OpenBootloader/Target/spi_interface.h              Header of SPI interface file
     - OpenBootloader/Target/interfaces_conf.h            Contains Interfaces configuration
     - OpenBootloader/Target/iwdg_interface.c             Contains IWDG interface
     - OpenBootloader/Target/iwdg_interface.h             Header of IWDG interface file
     - OpenBootloader/Target/openbootloader_conf.h        Header file that contains OpenBootloader HW dependent configuration
     - OpenBootloader/Target/optionbytes_interface.c      Contains OptionBytes interface
     - OpenBootloader/Target/optionbytes_interface.h      Header of OptionBytes interface file
     - OpenBootloader/Target/otp_interface.c              Contains OTP interface
     - OpenBootloader/Target/otp_interface.h              Header of OTP interface file
     - OpenBootloader/Target/platform.h                   Header for patterns
     - OpenBootloader/Target/ram_interface.c              Contains RAM interface
     - OpenBootloader/Target/ram_interface.h              Header of RAM interface file
     - OpenBootloader/Target/systemmemory_interface.c     Contains system memory interface
     - OpenBootloader/Target/systemmemory_interface.h     Header of system memory interface file
     - OpenBootloader/Target/usart_interface.c            Contains USART interface
     - OpenBootloader/Target/usart_interface.h            Header of USART interface file
     - OpenBootloader/Target/usb_interface.c              Contains USB interface
     - OpenBootloader/Target/usb_interface.h              Header of USB interface file
     - USB_Device/App/usb_device.c                        USB Device applicative file
     - USB_Device/App/usb_device.h                        USB Device applicative header file
     - USB_Device/App/usbd_desc.c                         USB Device descriptor file
     - USB_Device/App/usbd_desc.h                         USB Device descriptor header file
     - USB_Device/App/usbd_dfu_if.c                       USB Device interface applicative file
     - USB_Device/App/usbd_dfu_if.h                       USB Device interface applicative header file
     - USB_Device/Target/usbd_conf.c                      Contains user defines for USB configuration
     - USB_Device/Target/usbd_conf.h                      USB configuration header file

### <b>Hardware and Software environment</b>

  - This application runs on STM32H7S78 devices without security enabled (TZEN=0).

  - This application supports two workspaces:
    - STM32H7S78-DK_Flash workspace application running in FLASH.
    - STM32H7S78-DK_Ram workspace application running in RAM

  - This application has been tested with STMicroelectronics STM32H7S78-DK (MB1736A)
    board and can be easily tailored to any other supported device
    and development board.

  - STM32H7S78-DK set-up to use FDCAN2:
    - Connect the PB12 (CN2: 6), PB13 (CN2: 7) respectively to RX, TX pins of an external FDCAN transceiver
    - Connect CAN-H and CAN-L of the external FDCAN transceiver to your FDCAN Host adapter

  - STM32H7S78-DK set-up to use USART:
    - To use the USART4 for communication you have to connect:
      - Tx pin of your host adapter to PA1 (CN10: 1) pin of the STM32H7S78-DK board
      - Rx pin of your host adapter to PA0 (CN10: 2) pin of the STM32H7S78-DK board
    - These pins are already connected to the board embedded ST-Link

  - STM32H7S78-DK set-up to use I2C:
    - Set I2C address to 0xB0
    - To use the I2C1 for communication you have to connect:
      - SCL (CN15: 10) pin of your host adapter to PB6 pin of the STM32H7S78-DK board
      - SDA (CN15: 9) pin of your host adapter to PB7 pin of the STM32H7S78-DK board

  - STM32H7S78-DK set-up to use I3C:
    - To use the I3C1 for communication you have to connect:
    - To use the I3C1 for communication you have to connect:
      - SCL (CN15: 10) pin of your host adapter to PB6 pin of the STM32H7S78-DK board
      - SDA (CN15: 9) pin of your host adapter to PB7 pin of the STM32H7S78-DK board

  - STM32H7S78-DK set-up to use SPI
      - To use the SPI4 for communication you have to connect:
        - SCK  pin of your host adapter to PE12 (CN3: 12) pin
        - MISO pin of your host adapter to PE14 (CN3: 9) pin
        - MOSI pin of your host adapter to PE13 (CN3: 8) pin

  - STM32H7S78-DK set-up to use USB:
    - USB FS CN17

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the application
  - Run STM32CubeProgrammer and connect to OpenBootloader using USART4</br>
    Or
  - Connect your FDCAN host adapter and connect to OpenBootloader using FDCAN2</br>
    Or
  - Connect your I2C host adapter and connect to OpenBootloader using I2C1</br>
    Or
  - Connect your I3C host adapter and connect to OpenBootloader using I3C1</br>
    Or
  - Connect your SPI host adapter and connect to OpenBootloader using SPI4</br>
    Or
  - Connect your USB host adapter and connect to OpenBootloader using USB FS</br>
