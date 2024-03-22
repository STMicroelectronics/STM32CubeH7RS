## <b>ADC_Oversampling_Init Example Description</b>

How to use an ADC peripheral with oversampling.
This example is based on the STM32H7RSxx ADC HAL API.
The peripheral initialization done using LL unitary service functions for
optimization purposes (performance and size).

Description of ADC oversampling:

Perform automatically multiple successive ADC conversions and average
computation, by ADC hardware, and therefore off-load the CPU for the
equivalent task.
This feature can be used for the functions: averaging, data rate reduction,
SNR improvement, basic filtering.

This example uses 3 configurations of oversampling, for comparison
of conversion data and evaluation of oversampling benefits.


Example configuration:

ADC is configured to convert a single channel, in single conversion mode,
from SW trigger.

ADC oversampling feature is used with 3 settings:

- ADC oversampling enabled: ratio 16, bits right shift 4.
- ADC oversampling enabled: ratio 16, no bits right shift.
- ADC oversampling disabled.

Example execution:

From the main program execution, the ADC group regular converts the
selected channel successively with the 3 oversampling settings.

Then, data are evaluated:

- data range validity check
- user can evaluate the expected results of oversampling:
  ADC conversion data with oversampling enabled has less variation
  than with oversampling disabled.
- SW calculation of oversampled conversion data on 16 bits to obtain an accurate
  data equivalent to resolution 12 bits with floating point.

When all ADC conversions are completed, the cycle of 3 conversions restarts.

LD1 is used to monitor program execution status:

- Normal operation: LED toggle at 1Hz (toggle at the end of conversions cycle)
- Error: LED remaining turned on

Debug: variables to monitor with debugger:

- "uhADCxConvertedData_OVS_ratio16_shift4": ADC group regular conversion data, oversampling ratio 16 and shift 4 (data scale: 12 bits)
- "uhADCxConvertedData_OVS_ratio16_shift0": ADC group regular conversion data, oversampling ratio 16 and shift 0 (data scale: 16 bits)
- "uhADCxConvertedData_OVS_disabled":       ADC group regular conversion data, oversampling disabled (data scale corresponds to ADC resolution: 12 bits)
- "fConvertedData_OVS_EquivalentValue12bits": Calculation of oversampling raw data on 16 bits (from variable "uhADCxConvertedData_OVS_ratio16_shift0") to the equivalent data on resolution 12 bits with floating point.

Connection needed:

Use an external power supply to generate a voltage in range [0V; 3.3V]
and connect it to analog input pin (cf pin below).

Note: If no power supply available, this pin can be let floating (in this case
      ADC conversion data will be undetermined).

Other peripherals used:

  1 GPIO for LD1

  1 GPIO for analog input: ADC channel 10 on pin PC.00 (Arduino A1 pin 3 CN9, Morpho pin 38 CN11)

### <b>Keywords</b>

ADC, analog digital converter, analog, conversion, voltage, channel, analog input, oversampling

### <b>Directory contents</b>

  - ADC/ADC_Oversampling_Init/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - ADC/ADC_Oversampling_Init/Boot/Inc/main.h                  Header for main.c module
  - ADC/ADC_Oversampling_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - ADC/ADC_Oversampling_Init/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - ADC/ADC_Oversampling_Init/Boot/Src/main.c                  Main program
  - ADC/ADC_Oversampling_Init/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

