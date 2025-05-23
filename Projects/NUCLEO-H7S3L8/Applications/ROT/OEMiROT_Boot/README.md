## <b>OEMiROT_Boot application Description</b>

This project aims to provide an OEMiROT boot example.

OEMiRoT stands for OEM immutable (unchangeable) Root of Trust and acts as a first boot stage. OEMiRoT is an example based on the MCUboot open-source software. OEMiRoT offers two services:

- The Secure Boot (root of trust service) is an immutable code, which is always executed after a system reset. It checks static protections (options bytes),
  activates runtime protections, and then verifies the authenticity and integrity of the user application code before every execution.
- The Secure Firmware Update application is an immutable code that detects new firmware image candidate. It checks its version (version downgrade prevention),
  authenticity, and integrity before installing it after decryption.

An OEM updatable RoT (OEMuRoT) can be generated by compiling OEMiRoT_Boot project with OEMUROT_ENABLE switch activated. OEMuRoT acts as a second boot stage after STiRoT,
and provides the same two services: Secure Boot and Secure Firmware Update. Both OEMiRoT and OEMuRoT examples are generated based on OEMiRoT_Boot project with the switch OEMUROT_ENABLE
activated or not.

Please follow the article [<b>How to create ROT examples for STM32H7RS</b>](https://wiki.st.com/stm32mcu/wiki/Security:How_to_create_ROT_examples_for_STM32H7RS)
to generate this example based on `Firmware/Projects/STM32H7S78-DK/Applications/ROT/OEMiROT_Boot` project.
