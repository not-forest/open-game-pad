/* 
 *  Main firmware code for 'Open Game Pad'
 *
 *  This code defines all necessary functions and stuctures for operating data between input sensors and output USB connector.
 *  The code is interrupt driven, therefore all necessary initialization part can be found in main function, while the rest is
 *  located in ISR definitions. V-USB library is a main driver for communicating with a host device as a hid device.
 *
 * */
 
#include<avr/pgmspace.h>
#include<avr/interrupt.h>

#include<avr/delay.h>
#include<avr/wdt.h>
#include<avr/io.h>

#include "../usbdrv/usbdrv.h"
#include "ogpad.h"

// Pointer to the REPORT structure.
static report_t *RPTR = &REPORT;
// Determines how often the device should send a report to the host when there is no change in the state of the inputs.
static uchar IDLE_RATE;
// Inpur counter allows to define which key we are reading as a digital input or which axis as an analog input.
static volatile inputCounter ic = { .raw = 0 };

/* 
 * Game Pad report descriptor. 
 *
 * It defines a device's buttons and joysticks via USB report descriptor. 
 * */
PROGMEM const char usbDescriptorHidReport[REPORT_DESCRIPTOR_SIZE] = {
    // Global information.
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Gamepad)
    0xA1, 0x01,                    // COLLECTION (Application)
    0xA1, 0x00,                    //   COLLECTION (Physical)
    // 18 buttons handling.
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x12,                    //     USAGE_MAXIMUM (Button 18)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x12,                    //     REPORT_COUNT (18)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    // Unused bits are ignored.
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x0D,                    //     REPORT_SIZE (14)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    // Joysticks handling.
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x32,                    //     USAGE (Z)
    0x09, 0x33,                    //     USAGE (Rx)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7F,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x04,                    //     REPORT_COUNT (4)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    // Closing collections.
    0xC0,                          //   END_COLLECTION
    0xC0                           // END_COLLECTION
};

// This is the function from the V-USB library that must be defined here to properly handle the requests from the host.
usbMsgLen_t usbFunctionSetup(uchar raw[8]) {
    usbRequest_t *req = (void *) raw;

    // Class request type.
    if((req->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
        if(req->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            // we only have one report type, so don't look at wValue
            usbMsgPtr = (unsigned short) (void *) RPTR;
            return sizeof(REPORT);
        }else if(req->bRequest == USBRQ_HID_GET_IDLE){
            usbMsgPtr = (unsigned short) &IDLE_RATE;
            return 1;
        }else if(req->bRequest == USBRQ_HID_SET_IDLE){
            IDLE_RATE = req->wValue.bytes[1];
        }
    }else{
        /* Vendor specific requests could be implemented here to built up the firmware abilities. */
    } 

    return 0;
}

// Main function that initializes registers with required values and then waits for interrupts.
int __attribute__((noreturn)) main(void) {
    /*      GPIO Configuration      */
    // PB1, PB2 lines are handled by V-USB. PB0 is a digital input line by default. 
    // The clock is being bit-banged inside the ADC interrupt handler.
    DDRB = 1 << PB4;        // CLK output.
    PCMSK = 1 << PCINT0;    // The PCINT interrupt will be invoked only when something changes on PB0.

    /*      ADC Configuration       */
    // - Only 8 highest bits are required, therefore 1 Mhz sampling can be used for faster conversion. This sampling is a limit declared 
    // in the datasheet;
    // - Holding high bits in ADCH as a result;
    // - Single ended input with internal Vcc voltage reference is being used;
    ADCSRA = (1 << ADPS2) | (1 << ADSC) | (1 << ADIF);
    ADMUX = (1 << MUX1) | (1 << MUX0) | (1 << ADLAR);

    wdt_enable(WDTO_1S);                   // Enabling the watchdog timer and selecting the 1s expiring.
    usbInit();                             // Start of USB handling.
    
    usbDeviceDisconnect();                 // Forcing re-enumeration.
    wdt_reset();                           // One second is enough for the next step.
    _delay_ms(300);                        // USB would be disconnected for 300 ms.
    usbDeviceConnect();

    GIMSK = 1 << PCIE;
    // Main loop only handles the USB connection and resets the watchdog timer.
    sei();
    for(;;) {
        wdt_reset();
        usbPoll();                         // Polling the USB lines
        
        // Here we are sending the current data we have.
        if(usbInterruptIsReady()) {        // If interrupt is ready, sending the newest data.
            usbSetInterrupt((void *) RPTR, sizeof(REPORT));
        }
    }
}

/* 
 * This interrupt handles ADC data on AIN line. 
 *
 * It also does the output clock control by bitbanging, so all outside components are dependent on
 * ADC convertion. This way it is possible to decode all digital and analog inputs.
 * */
ISR(ADC_vect) {
    PORTB ^= (1 << PB4);                          // Clock tick.
    RPTR->joyax[ic.ANALOG] = ADCH;                // Writing the next analog input to the proper location.
    ADCSRA |= (1 << ADSC);                        // Starting new ADC conversion.
    ic.raw++;

    if(ic.raw > 18) ic.raw = 0;
}

/* 
 * Sets an interrupt for the PB0 pin. 
 * 
 * INT0 is reserved for the V-USB use. The PCINT[5..0] would cause this interrupt.
 * This handler detects a keypress obtained from the key matrix and iterates over each key to
 * define which key was pressed. All pressed keys are then saved into a mask and send via USB
 * connection.
 * */
ISR(PCINT0_vect) {
    RPTR->bmask = (PORTB & 1) << ic.DIGITAL;      // Marking the value in the button mask. This way several keys can be pressed in one loop.
}
