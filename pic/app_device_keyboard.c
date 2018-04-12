/*******************************************************************************
  USB device keyboard demo source file

  Company:
    Microchip Technology Inc.

  File Name:
    keyboard.c

  Summary:
    This demo will make the USB peripheral look like a USB keyboard.

  Description:
    This demo will make the USB peripheral look like a USB keyboard.  The code
    defines the behavior of the keyboard and how it operates.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <system.h>
#include <usb/usb.h>
#include <usb/usb_device_hid.h>

#if 0
#include "app_led_usb_status.h"
#endif


#define	MAXKEYBLOCK	2
#define	MAXKEYSCAN	7
#define	MAXKEYSENSE	6
static	const	unsigned short	keymap[MAXKEYBLOCK][MAXKEYSCAN][MAXKEYSENSE] = {
	{	/* left: B0 B1 B2 B3 B4 B5 */
		{0x0000, 0x0017, 0x0022, 0x0000, 0x0029, 0x002b}, /* A0 */
		{0x0019, 0x0009, 0x0015, 0x0021, 0x000a, 0x0005}, /* A4 */
		{0x004d, 0x0007, 0x0008, 0x0020, 0x0006, 0x0000}, /* C0 */
		{0x004a, 0x0004, 0x0014, 0x001e, 0x001d, 0x0000}, /* C1 */
		{0x0039, 0x002c, 0x0000, 0x0000, 0x0200, 0x008b}, /* C2 */
		{0x0035, 0x002f, 0x0032, 0x0030, 0x0400, 0x0100}, /* C6 */
		{0x004e, 0x0016, 0x001a, 0x001f, 0x001b, 0x004b} /* C7 */
	}, {	/* right: B0 B1 B2 B3 B4 B5 */
		{0x0028, 0x0000, 0x001c, 0x0023, 0x0049, 0x0000}, /* A0 */
		{0x0011, 0x0010, 0x000d, 0x0018, 0x000b, 0x0024}, /* A4 */
		{0x0052, 0x0051, 0x000f, 0x0012, 0x0037, 0x0026}, /* C0 */
		{0x0000, 0x004f, 0x0033, 0x0013, 0x0038, 0x0027}, /* C1 */
		{0x1000, 0x004c, 0x0089, 0x002d, 0x4000, 0x002e}, /* C2 */
		{0x0000, 0x0050, 0x000e, 0x000c, 0x0036, 0x0025}, /* C6 */
		{0x008a, 0x0088, 0x2000, 0x0000, 0x002a, 0x0000} /* C7 */
	}
};

// *****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Constants
// *****************************************************************************
// *****************************************************************************

//Class specific descriptor - HID Keyboard
const struct{uint8_t report[HID_RPT01_SIZE];}hid_rpt01={
{   0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0xff,                    //   LOGICAL_MAXIMUM (255)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0xff,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0}                          // End Collection
};


// *****************************************************************************
// *****************************************************************************
// Section: File Scope Data Types
// *****************************************************************************
// *****************************************************************************

/* This typedef defines the only INPUT report found in the HID report
 * descriptor and gives an easy way to create the OUTPUT report. */
typedef struct __attribute__((packed))
{
    /* The union below represents the first byte of the INPUT report.  It is
     * formed by the following HID report items:
     *
     *  0x19, 0xe0, //   USAGE_MINIMUM (Keyboard LeftControl)
     *  0x29, 0xe7, //   USAGE_MAXIMUM (Keyboard Right GUI)
     *  0x15, 0x00, //   LOGICAL_MINIMUM (0)
     *  0x25, 0x01, //   LOGICAL_MAXIMUM (1)
     *  0x75, 0x01, //   REPORT_SIZE (1)
     *  0x95, 0x08, //   REPORT_COUNT (8)
     *  0x81, 0x02, //   INPUT (Data,Var,Abs)
     *
     * The report size is 1 specifying 1 bit per entry.
     * The report count is 8 specifying there are 8 entries.
     * These entries represent the Usage items between Left Control (the usage
     * minimum) and Right GUI (the usage maximum).
     */
    union __attribute__((packed))
    {
        uint8_t value;
        struct __attribute__((packed))
        {
            unsigned leftControl    :1;
            unsigned leftShift      :1;
            unsigned leftAlt        :1;
            unsigned leftGUI        :1;
            unsigned rightControl   :1;
            unsigned rightShift     :1;
            unsigned rightAlt       :1;
            unsigned rightGUI       :1;
        } bits;
    } modifiers;

    /* There is one byte of constant data/padding that is specified in the
     * input report:
     *
     *  0x95, 0x01,                    //   REPORT_COUNT (1)
     *  0x75, 0x08,                    //   REPORT_SIZE (8)
     *  0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
     */
    unsigned :8;

    /* The last INPUT item in the INPUT report is an array type.  This array
     * contains an entry for each of the keys that are currently pressed until
     * the array limit, in this case 6 concurent key presses.
     *
     *  0x95, 0x06,                    //   REPORT_COUNT (6)
     *  0x75, 0x08,                    //   REPORT_SIZE (8)
     *  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
     *  0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
     *  0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
     *  0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
     *  0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
     *
     * Report count is 6 indicating that the array has 6 total entries.
     * Report size is 8 indicating each entry in the array is one byte.
     * The usage minimum indicates the lowest key value (Reserved/no event)
     * The usage maximum indicates the highest key value (Application button)
     * The logical minimum indicates the remapped value for the usage minimum:
     *   No Event has a logical value of 0.
     * The logical maximum indicates the remapped value for the usage maximum:
     *   Application button has a logical value of 101.
     *
     * In this case the logical min/max match the usage min/max so the logical
     * remapping doesn't actually change the values.
     *
     * To send a report with the 'a' key pressed (usage value of 0x04, logical
     * value in this example of 0x04 as well), then the array input would be the
     * following:
     *
     * LSB [0x04][0x00][0x00][0x00][0x00][0x00] MSB
     *
     * If the 'b' button was then pressed with the 'a' button still held down,
     * the report would then look like this:
     *
     * LSB [0x04][0x05][0x00][0x00][0x00][0x00] MSB
     *
     * If the 'a' button was then released with the 'b' button still held down,
     * the resulting array would be the following:
     *
     * LSB [0x05][0x00][0x00][0x00][0x00][0x00] MSB
     *
     * The 'a' key was removed from the array and all other items in the array
     * were shifted down. */
#define	MAXREPORTKEYS	6
    uint8_t keys[MAXREPORTKEYS];
} KEYBOARD_INPUT_REPORT;


/* This typedef defines the only OUTPUT report found in the HID report
 * descriptor and gives an easy way to parse the OUTPUT report. */
typedef union __attribute__((packed))
{
    /* The OUTPUT report is comprised of only one byte of data. */
    uint8_t value;
    struct
    {
        /* There are two report items that form the one byte of OUTPUT report
         * data.  The first report item defines 5 LED indicators:
         *
         *  0x95, 0x05,                    //   REPORT_COUNT (5)
         *  0x75, 0x01,                    //   REPORT_SIZE (1)
         *  0x05, 0x08,                    //   USAGE_PAGE (LEDs)
         *  0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
         *  0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
         *  0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
         *
         * The report count indicates there are 5 entries.
         * The report size is 1 indicating each entry is just one bit.
         * These items are located on the LED usage page
         * These items are all of the usages between Num Lock (the usage
         * minimum) and Kana (the usage maximum).
         */
        unsigned numLock        :1;
        unsigned capsLock       :1;
        unsigned scrollLock     :1;
        unsigned compose        :1;
        unsigned kana           :1;

        /* The second OUTPUT report item defines 3 bits of constant data
         * (padding) used to make a complete byte:
         *
         *  0x95, 0x01,                    //   REPORT_COUNT (1)
         *  0x75, 0x03,                    //   REPORT_SIZE (3)
         *  0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
         *
         * Report count of 1 indicates that there is one entry
         * Report size of 3 indicates the entry is 3 bits long. */
        unsigned                :3;
    } leds;
} KEYBOARD_OUTPUT_REPORT;


/* This creates a storage type for all of the information required to track the
 * current state of the keyboard. */
typedef struct
{
    USB_HANDLE lastINTransmission;
    USB_HANDLE lastOUTTransmission;
#if 0
    unsigned char key;
    bool waitingForRelease;
#endif
} KEYBOARD;

// *****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Variables
// *****************************************************************************
// *****************************************************************************
static KEYBOARD keyboard;

#if !defined(KEYBOARD_INPUT_REPORT_DATA_BUFFER_ADDRESS_TAG)
    #define KEYBOARD_INPUT_REPORT_DATA_BUFFER_ADDRESS_TAG
#endif
static KEYBOARD_INPUT_REPORT inputReport KEYBOARD_INPUT_REPORT_DATA_BUFFER_ADDRESS_TAG;

#if !defined(KEYBOARD_OUTPUT_REPORT_DATA_BUFFER_ADDRESS_TAG)
    #define KEYBOARD_OUTPUT_REPORT_DATA_BUFFER_ADDRESS_TAG
#endif
static volatile KEYBOARD_OUTPUT_REPORT outputReport KEYBOARD_OUTPUT_REPORT_DATA_BUFFER_ADDRESS_TAG;


// *****************************************************************************
// *****************************************************************************
// Section: Private Prototypes
// *****************************************************************************
// *****************************************************************************
static void APP_KeyboardProcessOutputReport(void);


// *****************************************************************************
// *****************************************************************************
// Section: Macros or Functions
// *****************************************************************************
// *****************************************************************************
void APP_KeyboardInit(void)
{
    //initialize the variable holding the handle for the last
    // transmission
    keyboard.lastINTransmission = 0;
    
    //enable the HID endpoint
    USBEnableEndpoint(HID_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    //Arm OUT endpoint so we can receive caps lock, num lock, etc. info from host
    keyboard.lastOUTTransmission = HIDRxPacket(HID_EP,(uint8_t*)&outputReport, sizeof(outputReport) );
}


static	void	addkey(unsigned short code)
{
	static KEYBOARD_INPUT_REPORT inputreportwork;
	static	signed char	pos = 0;
	
	if (code == 0xffff) {
		while (pos < MAXREPORTKEYS)
			inputreportwork.keys[pos++] = 0;
		
		inputReport = inputreportwork;
		
		pos = 0;
		inputreportwork.modifiers.value = 0;
		return;
	}
	inputreportwork.modifiers.value |= (code >> 8);
	if ((code & 0xff)&&(pos < MAXREPORTKEYS))
		inputreportwork.keys[pos++] = (code & 0xff);
}


void	keyscan(void)
{
	static	short	phase = -1;
	signed char	i, j;
	
	if (phase < 0) {
		ADCON1 = 0xf;
		CMCON = 7;
		INTCON2 = 0;		/* RBPU-enable */
		LATA = 0xff;
		LATC = 0xff;
		TRISA = 0x00;
		TRISB = 0xff;
		TRISC = 0x00;
		
		phase = 0;
	}
	if ((phase & 1)) {
		if (phase < 14) {
			i = (PORTEbits.RE3)? 0 : 1;
			j = phase >> 1;
			
			if (PORTBbits.RB0 == 0)
				addkey(keymap[i][j][0]);
			if (PORTBbits.RB1 == 0)
				addkey(keymap[i][j][1]);
			if (PORTBbits.RB2 == 0)
				addkey(keymap[i][j][2]);
			if (PORTBbits.RB3 == 0)
				addkey(keymap[i][j][3]);
			if (PORTBbits.RB4 == 0)
				addkey(keymap[i][j][4]);
			if (PORTBbits.RB5 == 0)
				addkey(keymap[i][j][5]);
		}
		LATA = 0xff;
		LATC = 0xff;
		TRISA = 0x00;
		TRISB = 0xff;
		TRISC = 0x00;
	} else {
		switch (phase) {
			case	0:
				TRISA = 0x10;
				TRISC = 0xc7;
				LATA = 0x00;
				break;
			case	2:
				TRISA = 0x01;
				TRISC = 0xc7;
				LATA = 0x00;
				break;
			case	4:
				TRISA = 0x11;
				TRISC = 0xc6;
				LATC = 0x00;
				break;
			case	6:
				TRISA = 0x11;
				TRISC = 0xc5;
				LATC = 0x00;
				break;
			case	8:
				TRISA = 0x11;
				TRISC = 0xc3;
				LATC = 0x00;
				break;
			case	10:
				TRISA = 0x11;
				TRISC = 0x87;
				LATC = 0x00;
				break;
			case	12:
				TRISA = 0x11;
				TRISC = 0x47;
				LATC = 0x00;
				break;
			default:
				phase = -1;
				addkey(0xffff);
				break;
		}
	}
	phase++;
}


void	APP_KeyboardTasks(void)
{
	if (HIDTxHandleBusy(keyboard.lastINTransmission) == false)
		keyboard.lastINTransmission = HIDTxPacket(HID_EP, (uint8_t*)&inputReport, sizeof(inputReport));
	
	if (HIDRxHandleBusy(keyboard.lastOUTTransmission) == false) {
		APP_KeyboardProcessOutputReport();
		keyboard.lastOUTTransmission = HIDRxPacket(HID_EP,(uint8_t*)&outputReport,sizeof(outputReport));
	}
}


static void APP_KeyboardProcessOutputReport(void)
{
}


static void USBHIDCBSetReportComplete(void)
{
    /* 1 byte of LED state data should now be in the CtrlTrfData buffer.  Copy
     * it to the OUTPUT report buffer for processing */
    outputReport.value = CtrlTrfData[0];

    /* Process the OUTPUT report. */
    APP_KeyboardProcessOutputReport();
}

void USBHIDCBSetReportHandler(void)
{
    /* Prepare to receive the keyboard LED state data through a SET_REPORT
     * control transfer on endpoint 0.  The host should only send 1 byte,
     * since this is all that the report descriptor allows it to send. */
    USBEP0Receive((uint8_t*)&CtrlTrfData, USB_EP0_BUFF_SIZE, USBHIDCBSetReportComplete);
}

/*******************************************************************************
 End of File
*/
