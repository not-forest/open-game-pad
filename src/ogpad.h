/* 
 *  Header file that defines all required constants for 'Open Game Padu' main code.
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
    // Left joystick's horizontal and vertical axises.
    int8_t lx;
    int8_t ly;
    // Right joystick's horizontal and vertical axises. 
    int8_t rx;
    int8_t ry;
} report_t;

#endif
