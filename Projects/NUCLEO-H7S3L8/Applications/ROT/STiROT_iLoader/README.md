## <b>STiROT_iLoader Application Description</b>

This project provides an immutable loader example required for <b>STiROT boot path</b> or <b>STiROT_OEMuROT boot path</b>.

STiRoT, which is an immutable code, is designed to be independent from external flash memory. Therefore,
the control of the authenticity and integrity of the user application is done by STiRoT from the internal RAM.
STiROT_iLoader project is provided to do the interface between the internal RAM and the external flash memory. This
project can be adapted to the external flash memory selected by the customer.

Please follow the article [<b>How to create ROT examples for STM32H7RS</b>](https://wiki.st.com/stm32mcu/wiki/Security:How_to_create_ROT_examples_for_STM32H7RS)
to generate this example based on `Firmware/Projects/STM32H7S78-DK/Applications/ROT/STiROT_iLoader` project.
