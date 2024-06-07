/* 
 *  Main firmware code for 'Open Game Pad'
 *
 *  This code defines all necessary functions and stuctures for operating data between input sensors and output USB connector.
 *  The code is interrupt driven, therefore all necessary initialization part can be found in main function, while the rest is
 *  located in ISR definitions. V-USB library is a main driver for communicating with a host device as a hid device.
 * */

#define F_CPU 16000000
 
#include<avr/io.h>
#include<avr/pgmspace.h>
#include<avr/interrupt.h>
#include<avr/delay.h>

#include "../usbdrv/usbdrv.h"
#include "ogpad.h"

/* 
 * Game Pad report descriptor. 
 *
 * It defines a device 
 * */
PROGMEM const char usbDescriptorHidReport[] = {
    0x05, 0x01,                     // Usage Page (Generic Desktop)
    0x09, 0x05,                     // Usage (Game Pad)
    0xa1, 0x01,                     // Collection (Application)
        // First Joystick (Joystick 1)
        0x09, 0x01,                 //      USAGE (Pointer)
        0xa1, 0x00,                 //      COLLECTION (Physical)
            0x09, 0x30,             //          USAGE (X)
            0x09, 0x31,             //          USAGE (Y)
            0x15, 0x00,             //          LOGICAL MINIMUM (0)
            0x26, 0xff, 0x00,       //          LOGICAL MAXIMUM (255)
            0x75, 0x08,             //          REPORT SIZE (8)
            0x95, 0x02,             //          REPORT COUNT (2)
            0x81, 0x02,             //          INPUT (Data,Var,Abs)
        0xc0,                       //      END_COLLECTION
        // Second Joystick (Joystick 2)
        0x09, 0x01,                 //      USAGE (Pointer)
        0xa1, 0x00,                 //      COLLECTION (Physical)
            0x09, 0x32,             //          USAGE (Z) - Use for 2nd Joystick X
            0x09, 0x35,             //          USAGE (Rz) - Use for 2nd Joystick Y
            0x15, 0x00,             //          LOGICAL MINIMUM (0)
            0x26, 0xff, 0x00,       //          LOGICAL MAXIMUM (255)
            0x75, 0x08,             //          REPORT SIZE (8)
            0x95, 0x02,             //          REPORT COUNT (2)
            0x81, 0x02,             //          INPUT (Data,Var,Abs)
        0xc0,                       //      END_COLLECTION
    // Buttons
    0x05, 0x09,                     // Usage Page (Button)
    0x19, 0x01,                     // Usage Minimum (Button 1)
    0x29, 0x10,                     // Usage Maximum (Button 16)
    0x15, 0x00,                     // Logical Minimum (0)
    0x25, 0x01,                     // Logical Maximum (1)
    0x75, 0x01,                     // Report Size (1)
    0x95, 0x10,                     // Report Count (16)
    0x81, 0x02,                     // Input (Data,Var,Abs)
    0xc0                            // End Collection
};


// Main function that initializes registers with required values and then waits for interrupts.
int __attribute__((noreturn)) main(void) {
    // TODO!
    for(;;);
}
