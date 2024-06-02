#!/bin/bash

# Microcontroller type
MCU=attiny85
# Set the CPU frequency
F_CPU=8000000
# Set the input C file and desired output file names
INPUT="src/main.c"
OUTPUT="main"

CFLAGS="-mmcu=$MCU -DF_CPU=$F_CPU -Wall -Os"

echo "Compiling $INPUT -> $OUTPUT.elf..."
avr-gcc $CFLAGS -o $OUTPUT.elf $INPUT

if [ $? -eq 0 ]; then
    echo "Creating HEX file..."
    avr-objcopy -O ihex -R .eeprom $OUTPUT.elf $OUTPUT.hex
    echo "Compilation successful."
else
    echo "Compilation failed."
fi
