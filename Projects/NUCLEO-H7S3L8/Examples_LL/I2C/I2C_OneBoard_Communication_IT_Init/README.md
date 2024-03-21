## <b>I2C_OneBoard_Communication_IT_Init Example Description</b>

How to handle the reception of one data byte from an I2C slave device
by an I2C master device. Both devices operate in interrupt mode.

The peripheral is initialized with LL initialization function to demonstrate LL init usage.

At the beginning of the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 600 MHz.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

This example guides you through the different configuration steps by mean of LL API
to configure GPIO and I2C peripherals using only one NUCLEO-H7S3L8.

The user can disable internal pull-up by opening ioc file.

For that, user can follow the procedure :

1. Double click on the I2C_OneBoard_Communication_IT_Init.ioc file

2. When CUBEMX tool is opened, select System Core category

3. Then in the configuration of GPIO/I2C1, change No pull-up and no pull-down to Pull-up for the both pins

4. Same action in the configuration of GPIO/I2C2, change No pull-up and no pull-down to Pull-up for the both pins

5. Last step, generate new code thanks to button "GENERATE CODE"

The example is updated with no pull on each pin used for I2C communication.

I2C1 Peripheral is configured in Slave mode with EXTI (Clock 600Khz, Own address 7-bit enabled). 

I2C2 Peripheral is configured in Master mode with EXTI (Clock 600Khz). 

GPIO associated to User push-button is linked with EXTI.

LD1 blinks quickly to wait for user-button press.

Example execution:

Press the USER push-button to initiate a read request by Master.

This action will generate an I2C start condition with the Slave address and a read bit condition.

When address Slave match code is received on I2C1, an ADDR interrupt occurs. 

I2C1 IRQ Handler routine is then checking Address Match Code and direction Read. 

This will allow Slave to enter in transmitter mode and then send a byte when TXIS interrupt occurs. 

When byte is received on I2C2, an RXNE interrupt occurs. 

When RXDR register is read, Master auto-generate a NACK and STOP condition 
to inform the Slave that the transfer is finished. 

The NACK condition generate a NACK interrupt in Slave side treated in the I2C1 IRQ handler routine by a clear of NACK flag.

The STOP condition generate a STOP interrupt in both side (Slave and Master). I2C1 and I2C2 IRQ handler routine are then
clearing the STOP flag in both side.

LD1 is On if data is well received.

In case of errors, LD1 is blinking slowly (1s).

### <b>Keywords</b>

Connectivity, Communication, I2C, Master, Slave, Transmission, Reception, Fast mode


#### <b>Notes</b>

 1. If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),
    it is recommended to configure the latters as Write Through.
    This is ensured by configuring the memory attributes at MPU level in order to ensure cache coherence on SRAM1 and SRAM2.
    Please, refer to Template project for a typical MPU configuration.

 2. If external memory is shared between several processors, it is recommended to configure it as Write Back (bufferable), shareable and cacheable.
    The memory base address and size must be properly updated.
    The user needs to manage the cache coherence at application level.

 3. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
    by the Cortex M7 and the GPDMA/HPDMA), there is no need for cache maintenance.
    If the application needs to put DMA buffers in AXI SRAM (starting from @0x24000000), the user has to:
    - either define a non-cacheable region in the MPU and linker configuration file to locate DMA buffers
      (a proposed dma_buffer section is available from CMSIS Device linker template file and its size must
      be adapted to the application requirements)
    - or to ensure cache maintenance operations to ensure the cache coherence between the CPU and the DMAs.
    This is true also for any other data buffers accessed by the CPU and other masters (DMA2D, LTDC)
    The addresses and the size of cacheable buffers (shared between CPU and other masters)
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"
    
For more details about the MPU configuration and use, please refer to AN4838 “Managing memory protection unit (MPU) in STM32 MCUs”

### <b>Directory contents</b>

  - I2C/I2C_OneBoard_Communication_IT_Init/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - I2C/I2C_OneBoard_Communication_IT_Init/Boot/Inc/main.h                    Header for main.c module
  - I2C/I2C_OneBoard_Communication_IT_Init/Boot/Inc/stm32_assert.h            Template file to include assert_failed function
  - I2C/I2C_OneBoard_Communication_IT_Init/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
  - I2C/I2C_OneBoard_Communication_IT_Init/Boot/Src/main.c                    Main program
  - I2C/I2C_OneBoard_Communication_IT_Init/Boot/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up
    - Connect GPIOs connected to I2C1 SCL/SDA (PB.8 and PB.9)
    to respectively SCL and SDA pins of I2C2 (PF.1 and PF.0).
      - I2C1_SCL  PB.8 (CN7, pin 2) : connected to I2C2_SCL PF.1 (CN9, pin 19)
      - I2C1_SDA  PB.9 (CN7, pin 4) : connected to I2C2_SDA PF.0 (CN9, pin 21)

  - Launch the program
  - Press USER push-button to initiate a read request by Master
      then Slave send a byte.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

