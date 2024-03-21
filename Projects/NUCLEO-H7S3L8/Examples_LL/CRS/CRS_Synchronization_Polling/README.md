## <b>CRS_Synchronization_Polling example Description</b>
How to configure the clock recovery system in polling mode through the
STM32H7RSxx CRS LL API. The peripheral initialization uses LL unitary
service functions for optimization purposes (performance and size).

### <b>Notes</b>

At the beginning of the main program the SystemClock_Config() function is used to
configure the system clock (SYSCLK) to run at 600 MHz.

Then, HSI48 and LSE oscillators are enabled.
In this example CRS will use LSE oscillator to calibrate HSI48 frequency.
When HSI48 and LSE are started, automatic calibration starts and program call the
polling function to wait for synchronization status.

In this test, there are results which could occur:

 - Wait for USER push-button press to start the test (Fast toggle LD1)
 - few loops occurs up to SYNC OK event (if SYNC WARN, HSI48 trimming value will be
 automatically modified)
 - in case of SYNC MISS or SYNC ERROR, there is an issue with synchronization input
 parameters. In this case, user need to apply new parameters after checking synchronization
 information and restart the synchronization.
 - LD1 is slowly blinking (1 sec. period) in case of timeout during polling
 procedure. It may be due to an issue with LSE synchronization frequency.
 - LD1 will finally power on if SYNC OK is returned by CRS.


### <b>Keywords</b>

CRS, automatic calibration, polling, HSI48, LSE

### <b>Directory contents</b>

      - CRS/CRS_Synchronization_Polling/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
      - CRS/CRS_Synchronization_Polling/Boot/Inc/main.h                  Header for main.c module
      - CRS/CRS_Synchronization_Polling/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
      - CRS/CRS_Synchronization_Polling/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
      - CRS/CRS_Synchronization_Polling/Boot/Src/main.c                  Main program
      - CRS/CRS_Synchronization_Polling/Boot/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up
    - Connect the MCO pin to an oscilloscope to monitor HSI48 frequency:
      - PA.08: connected to pin 23 of CN12 connector for Nucleo-144 (MB1737)

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
