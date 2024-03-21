## <b>CRC_CalculateAndCheck Example Description</b>

How to configure the CRC calculation unit to compute a CRC code for a given data
buffer, based on a fixed generator polynomial (default value 0x4C11DB7). The
peripheral initialization is done using LL unitary service functions for
optimization purposes (performance and size).

The CRC peripheral is configured to work with default polynomial value (32-bit long).
Normal representation of this polynomial value is :
 X^32 + X^26 + X^23 + X^22 + X^16 + X^12 + X^11 + X^10 +X^8 + X^7 + X^5 + X^4 + X^2 + X + 1.
Generated CRC value is then 32 bits long.

Example execution:
After startup from reset and system configuration, CRC configuration is performed (use of default Polynomial and initialisation values).
CRC code of a given data buffer is computed.
Data buffer length has been chosen as not an exact nb of u32 (32-bit words), in order to illustrate
use of offered API for feeding the calculator (u32, u16 or u8 inputs).
The calculated CRC code is stored in uwCRCValue variable.
Once calculated, CRC value (uwCRCValue) is compared to the CRC expected value (uwExpectedCRCValue),
and if both are equal, LD1 is turned On.
In case of errors, LD1 is blinking (1sec period).

### <b>Keywords</b>

Security, CRC, CRC Polynomial, ISO/IEC/IEEE 802-3, hardware CRC

### <b>Directory contents</b>

  - CRC/CRC_CalculateAndCheck/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - CRC/CRC_CalculateAndCheck/Boot/Inc/main.h                    Header for main.c module
  - CRC/CRC_CalculateAndCheck/Boot/Inc/stm32_assert.h            Template file to include assert_failed function
  - CRC/CRC_CalculateAndCheck/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
  - CRC/CRC_CalculateAndCheck/Boot/Src/main.c                    Main program
  - CRC/CRC_CalculateAndCheck/BootSrc/system_stm32h7rsxx.c      STM32H7RSxx system source file


### <b>Hardware and Software environment</b> 

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

