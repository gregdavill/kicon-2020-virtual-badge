# KiCon 2020 Virtual Badge by Thomas Flummer

![Render](RENDER/workshop_table_badge.jpg)

## Experimentation with tiny LEDs and LiPo charge and protection circuit

The electronics on this badge is mainly a few experiments with stuff that I would like to test and experiment a little with and then a bit of control parts to keep everything on a single board.

The two primary things I'm testing is:

- 1S LiPo charge and protection circuit
- Soldering and controlling very small (1x1mm) RGB LEDs

The microcontroller on the board is a Microchip SAMD21 and most of the controller circuit is actually copied from the [2020 BornHack badge](https://github.com/bornhack/badge2020).

Another experiment is reusing nested sheets for the schematic, and updating them to work with the nightly builds of KiCad, which actually went fairly smooth, though a bit of relinking symbols was needed.

## On board prototyping area

In addition to the microcontroller, LiPo control circuit and the LED drivers and LEDs, there are also a few buttons and all unused pins on the SAMD21 has been broken out to test points. This makes the badge useable as a test platform for other things and the combination of SMD and PTH proto pads allows for a bit of tinkering with various other parts or breakout boards, eg. sensors, screens or maybe more LEDs.

## Firmware for the SAMD21

My plan is to put [circuitpython](https://circuitpython.org/) on my badge, but an Arduino bootloader or maybe Zephyr could also be options. The extra SPI flash (which is optional), can be used to save circuitpython libraries and code and even though the SAMD21 isn't the most powerful controller, it's perfect for a bit of quick testing and tinkering.

## Making your own

If you want to make your own, I have included the gerbers, in case you don't want to install the nightly version.

Since some of the parts are a bit small I recommend getting a stencil for solderpaste and when applying, being very careful not to smudge the paste, especially on the USB connector and the two 0.4mm pitch QFN44 LED driver chips, since they are a bit prone to bridges.

## Please share

If you decide to make this badge or a variant of it, please share images so we can all see it and get inspired. If sharing on social media, please use the #badgelife tag and please let [me](https://thomasflummer.com/) know, I would love to see what you have made!
