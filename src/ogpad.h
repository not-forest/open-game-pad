/* 
 *  Header file that defines all required constants for 'Open Game Pad' main code.
 * */

#ifndef __OGPAD_H__
#define __OGPAD_H__

#include <stdint.h>

#define REPORT_DESCRIPTOR_SIZE 58

/* 
 *  Custom structure that describes data obtained from the game pad.
 *
 *  This report is then sent to the host device via USB protocol. The report format must be compatible with the HID report.
 * */
typedef struct {
    // Button mask for all 16 keys. Each bit corresponds to a key value.
    uint16_t bmask;
    // Only two bits matter.
    uint8_t joyb; 
    // The joystick axises are defined in the following order: left HORIZONTAL, left VERTICAL, right HORIZONTAL, right VERTICAL.
    int8_t joyax[4];
} __attribute__((packed)) report_t;

/* 
 *  Input Counter Byte. Counts which input (ANALOG/DIGITAL) is currently in
 *
 *  Custom union type that allows for using one 8-bit variable to manipulate with indexes of smaller sizes.
 *  This union allows to manipulate the data without bitwise operation, making it a better readibility choice. 
 *  Note that ANALOG and DIGITAL fields are READONLY must not never be used to write new data. */
typedef union {
    uint8_t raw;            // Full byte with five last bits always unused.
    
    uint8_t ANALOG: 2;      // Two lower bits for 4 analog inputs.
    uint8_t DIGITAL: 3;     // 3 lower bits for 18 digital inputs encoded in a 5-bit value MSB first. 
} inputCounter;

// Game Pad report holds the current pressed keys and joystick axises derivatives.
static report_t REPORT;

#endif
