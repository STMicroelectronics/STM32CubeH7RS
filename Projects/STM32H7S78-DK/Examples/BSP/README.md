## <b>BSP  Example Description</b>

- This example provides a description of how to use the different BSP drivers.

#### <b>Notes</b>

In this example, the code is executed from SRAM memory.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32H7S7xx Devices :
The CPU at 400MHz

This example shows how to use the different functionalities of components
available on the board by switching between all tests using USER push-button.

Push the User button to move from one test to another.

- TOUCHSCREEN Demo 1

Use touchscreen functionality to select or activate colored circle inside a rectangle.

- TOUCHSCREEN Demo 2

Use touchscreen functionality to select 2 touch points, their positions will be
captured and displayed on the LCD.

- TOUCHSCREEN Demo 3

Use touchscreen functionality to select or activate colored square in a grid.

- LCD

This example shows how to use the different LCD features to display string
with different fonts, to display different shapes and to draw a bitmap.

- AUDIO PLAY

This example shows how to play an audio file using the DMA circular mode and
how to handle the buffer update.
Plug a headphone to hear the sound
@Note: Copy file 'audio_sample_tdm.bin' (available in Binary) directly in
       the flash at @0x700A0000 using STM32CubeProgrammer utility

- AUDIO RECORD

This example shows how to record an audio stream from microphones mounted on STM32H7S78-DK
and play it using the DMA circular mode and how to handle the buffer update.
Plug a headphone to hear the sound.

- SD

This example shows how to erase, write and read in the SD memory.


### <b>Keywords</b>

BSP, Board support package, Button, Leds, Audio play, Headphone, Audio record, microphone,
Touchscreen, Display, LCD, LTDC, SDMMC.

### <b>Directory contents</b>

  - BSP/Inc/aps256xx_conf.h             Component configuration file
  - BSP/Inc/gt911_conf.h                Component configuration file
  - BSP/Inc/main.h                      Main program header file for Cortex-M7
  - BSP/Inc/mx25lm51245g_conf.h         Component configuration file
  - BSP/Inc/mx66uw1g45g_conf.h          Component configuration file
  - BSP/Inc/stlogo.h                    Image used for BSP example
  - BSP/Inc/stm32h7rsxx_hal_conf.h      HAL Configuration file for Cortex-M7
  - BSP/Inc/stm32h7rsxx_it.h            Interrupt handlers header file for Cortex-M7
  - BSP/Inc/stm32h7s78_discovery_conf.h BSP configuration file
  - BSP/Inc/system_stm32h7rsxx.h        STM32H7RSxx system configuration header file
  - BSP/Src/audio_play.c                Play audio
  - BSP/Src/audio_record.c              Record and Play audio
  - BSP/Src/lcd.c                       LCD drawing features
  - BSP/Src/main.c                      Main program for Cortex-M7
  - BSP/Src/sd.c                        SD Read/Write features
  - BSP/Src/stm32h7xx_hal_msp.c         HAL MSP module for Cortex-M7
  - BSP/Src/stm32h7xx_it.c              Interrupt handlers for Cortex-M7
  - BSP/Src/system_stm32h7rsxx.c        STM32H7RSxx system configuration source file
  - BSP/Src/ts.c                        Touchscreen features
  - BSP/Src/xspi.c                      XSPI features


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7RSxx devices.

  - This example has been tested with STM32H7S78-DK board and can be
    easily tailored to any other supported device and development board.

  STM32H7S78-DK Set-up :

  - Connect the uSD Card to the Micro SD connector (CN13).


### <b>How to use it ?</b>

 - Use STM32CubeProgrammer, available on www.st.com or any other in system programming
   tool to load "BSP/Binary/audio_sample_tdm.bin" file to the external QSPI flash
   at the address 0x700A0000.
       - Open the STM32CubeProgrammer tool
       - Select the octoSPI external flash loader "MX66UW1G45G_STM32H7S78-DK"
       - From Erasing & Programming menu, browse and open the output binary file relative to this example
       - Load the file into the external QSPI flash using "Start Programming" at the address APPLICATION_ADDRESS (0x700A0000)


In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Plug the SD Card in the STM32H7S78-DK
 - Run the example

