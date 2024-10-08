##############################################################################
# Open Game Pad compilation and fuse bits configurations.
##############################################################################

DEVICE  = attiny85
FUSE_L  = 0xa1
FUSE_H  = 0xdd

PORT    = /dev/ttyACM0
PROG    = stk500v1
BAUD	= 19200
AVRDUDE = avrdude -c $(PROG) -b $(BAUD) -p $(DEVICE) -P $(PORT)

F_CPU 	= 16500000L

CFLAGS  = -Iusbdrv -I. -DDEBUG_LEVEL=0
OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o src/main.o

COMPILE = avr-gcc -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -Wall -Os $(CFLAGS)

##############################################################################
#                                Fuse values                                 #
##############################################################################
# Made based on fuse calculator: https://www.engbedded.com/fusecalc/		 #
################################## ATtiny85 ##################################
# FUSE_L (Fuse low byte):
# 0xa2 = 1 0 1 0   0 0 0 1
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (PLL Clock source 16MHz)
#        | |  +--------------- SUT 1..0 (startup time + 64ms)
#        | +------------------ CKOUT (clock output on CKOUT pin -> enabled)
#        +-------------------- CKDIV8 (divide clock by 8 -> disabled)
# FUSE_H (Fuse high byte):
# 0xdd = 1 1 0 1   1 1 0 1
#        ^ ^ ^ ^   ^ \-+-/---- BODLEVEL 2..0 (brownout trigger level -> 2.7V)
#        | | | |   +---------- EESAVE (preserve EEPROM on Chip Erase -> disabled)
#        | | | +-------------- WDTON (watchdog timer always on -> disabled)
#        | | +---------------- SPIEN (enable serial programming -> enabled)
#        | +------------------ DWEN (enable debug wire -> disabled)
#        +-------------------- RSTDISBL (disabling external reset -> disabled)
##############################################################################

# symbolic targets:
help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make hex ....... to build main.hex"
	@echo "make program ... to flash fuses and firmware"
	@echo "make fuse ...... to flash the fuses"
	@echo "make flash ..... to flash the firmware"
	@echo "make clean ..... to delete objects and hex file"

hex: main.hex

program: flash fuse

# rule for programming fuse bits:
fuse:
	@[ "$(FUSE_H)" != "" -a "$(FUSE_L)" != "" ] || \
		{ echo "*** Edit Makefile and choose values for FUSE_L and FUSE_H!"; exit 1; }
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m

# rule for uploading firmware:
flash: main.hex
	$(AVRDUDE) -U flash:w:main.hex:i

# rule for deleting dependent files (those which can be built by Make):
clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.elf *.o src/*.o

# Generic rule for compiling C files:
.c.o:
	$(COMPILE) -c $< -o $@

# Generic rule for assembling Assembler source files:
.S.o:
	$(COMPILE) -c $< -o $@
# Generic rule for compiling C to assembler, used for debugging only.
.c.s:
	$(COMPILE) -S $< -o $@

# file targets:

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size main.hex

# debugging targets:

disasm:	main.elf
	avr-objdump -d main.elf
