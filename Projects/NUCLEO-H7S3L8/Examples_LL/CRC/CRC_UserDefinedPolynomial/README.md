## <b>CRC_UserDefinedPolynomial Example Description</b>

How to configure and use the CRC calculation unit to compute an 8-bit CRC code
for a given data buffer, based on a user-defined generating polynomial. The
peripheral initialization is done using LL unitary service functions for
optimization purposes (performance and size).

The CRC peripheral is configured to work with a user defined polynomial value (8-bit long value).
In this example, the polynomial is set manually to 0x9B that is X^8 + X^7 + X^4 + X^3 + X + 1.
Generated CRC value is then 8-bit long.

Example execution:
After startup from reset and system configuration, CRC configuration is performed (use of user-defined Polynomial value).
CRC code of a given data buffer is computed.
Data buffer length has been chosen as not an exact nb of u32 (32-bit words), in order to illustrate
use of offered API for feeding the calculator (u32, u16 or u8 inputs).
The calculated CRC code is stored in ubCRCValue variable.
Once calculated, CRC value (ubCRCValue) is compared to the CRC expected value (ubExpectedCRCValue),
and if both are equal, LD1 is turned On.
In case of errors, LD1 is blinking (1sec period).


### <b>Keywords</b>

Security, CRC, CRC Polynomial, CRC-8-WCDMA, hardware CRC, user-defined generating polynomial

### <b>Directory contents</b>

  - CRC/CRC_UserDefinedPolynomial/Boot/Inc/stm32h7rsxx_it.h              Interrupt handlers header file
  - CRC/CRC_UserDefinedPolynomial/Boot/Inc/main.h                        Header for main.c module
  - CRC/CRC_UserDefinedPolynomial/Boot/Inc/stm32_assert.h                Template file to include assert_failed function
  - CRC/CRC_UserDefinedPolynomial/Boot/Src/stm32h7rsxx_it.c              Interrupt handlers
  - CRC/CRC_UserDefinedPolynomial/Boot/Src/main.c                        Main program
  - CRC/CRC_UserDefinedPolynomial/Boot/Src/system_stm32h7rsxx.c          STM32H7RSxx system source file


### <b>Hardware and Software environment</b> 

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

