This note describes the idea behind an `Open Game Pad` project.
## Presumptions

The idea is to create a minimal working, low-budget, Open-Source game-pad that could be used as a reference for more high-level builds. The pad is using a most standard game-pad reference layout, however an Xbox-like layout variety is available in PCBs folder.

![[Pasted image 20240602210504.png]]
## Diagram

Below is a simple diagram of hardware logic behind this system. The ATtiny85 AVR family micro-controller is being used as a small, low-cost micro-controller, which is enough for the use case. We are going to use 2 out of 4 available ADC channels on one pin for joysticks analog inputs. 

The rest of the pins must be multiplexed by software. That lives us with 4 pins for 17 buttons, because one pin must be used for USB communication. Not enough by any means but still possible.

![[Open Game Pad Canvas.canvas|Open Game Pad Canvas]]
## Components required

- ATtiny85
- Any ATtiny programmer

## Hardware

TODO!
## Software

TODO!

Links:
- [[Open Game Pad README]]
- [[Open Game Pad Canvas.canvas|Open Game Pad Canvas]]
