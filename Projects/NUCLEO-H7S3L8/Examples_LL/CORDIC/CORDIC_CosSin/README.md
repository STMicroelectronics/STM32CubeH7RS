## <b>CORDIC_CosSin Example Description</b>

How to use the CORDIC peripheral to calculate cosine and sine.

This example is based on the STM32H7RSxx CORDIC LL API.
The peripheral initialization and control is done using LL unitary service
functions for optimization purpose (performance and size).

In this example, the CORDIC peripheral is configured in cosine function, q1.31
format for both input and output data, and with 6 cycles of precision.
The input data provided to CORDIC processor are angles in radians
divided by PI, in q1.31 format. The output data are cosine and sine in q1.31
format. For 6 cycles of precision, the maximal expected residual error of the
calculated sines is 2^-19.

The calculated cosine and sine are stored in cosOutput and sinOutput variables.
The residual error of calculation results is verified, by comparing to
reference values.

STM32 board LD1 is used to monitor the example status:

  - LD1 is ON when correct CORDIC cosine and sine results are calculated.
  - LD1 is blinking (2 second period) when exceeding residual error
    on CORDIC results is detected or when there is an initialization error.

### <b>Keywords</b>

CORDIC, sine, cosine

### <b>Directory contents</b>

  - CORDIC/CORDIC_CosSin/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - CORDIC/CORDIC_CosSin/Boot/Inc/main.h                  Header for main.c module
  - CORDIC/CORDIC_CosSin/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - CORDIC/CORDIC_CosSin/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - CORDIC/CORDIC_CosSin/Boot/Src/main.c                  Main program
  - CORDIC/CORDIC_CosSin/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

