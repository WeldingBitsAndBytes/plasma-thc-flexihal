# WIP

A THC control board that can connect to a Expatria Flexihal for controlling a plasma cutter


`thc_divider_board` directory contains a Kicad PCB designed to isolate the raw plasma cutter 
measurements from the MCU. The PCBs were designed with Oshpark in mind, in particular the
"After Dark" style just for fun. As such, there is minimal silkscreening.

![front](thc_divider_board/front.png)

![front](thc_divider_board/back.png)

The board is intended to be powered by an internal 24VDC power supply within the plasma cutter. The 
plasma cutter's power supply is protected by a 6KVDC reinforced DC-DC converter. The low-side of the 
isolation amplifier is powered by another isolation DC-DC converter fed from the main DC-DC 
converter. This secondary 5V DC-DC converter also provides power for the MCU.

The MCU is intended to be something like an Arduino Nano. The signal is connected to the ADC. 
The MCU is connected to a https://www.dfrobot.com/product-2588.html UART Fibre Optic Transceiver

The `teensy_quadrature_encoder` directory contains the sketch for a Teensy 4 to convert from
serial to quadrature encoding. The quadrature encoding is then used to interface with a Expatria
FlexiHAL and by extension LinuxCNC for torch height control.

More to come :)
