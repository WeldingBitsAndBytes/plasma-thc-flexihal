# WIP

A THC control board that can connect to a Expatria Flexihal for controlling a plasma cutter

## THC Divider Board

`thc_divider_board` directory contains a Kicad PCB designed to isolate the raw plasma cutter 
measurements from the MCU. The PCBs were designed with Oshpark in mind, in particular the
"After Dark" style just for fun. As such, there is minimal silkscreening.

![front](thc_divider_board/front.png)

![front](thc_divider_board/back.png)

The board is intended to be powered by an internal 24VDC power supply within the plasma cutter. The 
plasma cutter's power supply is protected by a 6KVDC reinforced DC-DC converter. The low-side of the 
isolation amplifier is powered by another isolation DC-DC converter fed from the main DC-DC 
converter. This secondary 5V DC-DC converter also provides power for the MCU.

The MCU is intended to be something like an Arduino Nano. Almost any other MCU would work but these
are cheap, small, and just fine for this application.

## Nano Voltage Sender

`nano_voltage_sender` contains the sketch for the Arduino Nano. It uses an external Vref voltage source
to normalize out the voltage for more accurate readings. It uses the 2V reference source to match the
0-2V output from the THC divider board.

The signal is connected to the ADC. The MCU is connected to a https://www.dfrobot.com/product-2588.html 
UART Fibre Optic Transceiver

## Teensy Quadrature Encoder

The `teensy_quadrature_encoder` directory contains the sketch for a Teensy 4 to convert from
serial to quadrature encoding. The Teensy 4 was chosen for its high speed and ability to measure nanoseconds 
for precise timings.  The quadrature encoding is then used to interface with a Expatria FlexiHAL and by 
extension LinuxCNC for torch height control. Similarly, this Teensy will also have a UART Fibre Optic 
Transceiver to talk to the Nano

## FlexiHAL Hookup and Configuration

More to come :)
