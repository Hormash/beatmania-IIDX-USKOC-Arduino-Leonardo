/*
  Joystick.cpp

  Copyright (c) 2015, Matthew Heironimus

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Joystick.h"

#if defined(_USING_HID)

#define JOYSTICK_REPORT_ID 0x03
#define JOYSTICK_STATE_SIZE 6

static const uint8_t _hidReportDescriptor[] PROGMEM = {
  
	// Joystick
	0x05, 0x01,			      // USAGE_PAGE (Generic Desktop)
	0x09, 0x04,			      // USAGE (Joystick)
	0xa1, 0x01,			      // COLLECTION (Application)
	0x85, JOYSTICK_REPORT_ID, //   REPORT_ID (3)

	// 32 Buttons
	0x05, 0x09,			      //   USAGE_PAGE (Button)
	0x19, 0x01,			      //   USAGE_MINIMUM (Button 1)
	0x29, 0x20,			      //   USAGE_MAXIMUM (Button 32)
	0x15, 0x00,			      //   LOGICAL_MINIMUM (0)
	0x25, 0x01,			      //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,			      //   REPORT_SIZE (1)
	0x95, 0x20,			      //   REPORT_COUNT (32)
	0x55, 0x00,			      //   UNIT_EXPONENT (0)
	0x65, 0x00,			      //   UNIT (None)
	0x81, 0x02,			      //   INPUT (Data,Var,Abs)

	// X, Y, and Z Axis
  0x05, 0x01,                    /*     USAGE_PAGE (Generic Desktop) */ 
  0x09, 0x30,                    /*     USAGE (X) */ 
  0x09, 0x31,                    /*     USAGE (Y) */ 
  0x15, 0x81,                    /*     LOGICAL_MINIMUM (0) */ 
  0x25, 0x7f,                    /*     LOGICAL_MAXIMUM (255) */ 
  0x95, 0x02,                    /*     REPORT_COUNT (3) */ 
  0x75, 0x08,                    /*     REPORT_SIZE (8) */ 
  0x81, 0x02,                    /*     INPUT (Data,Var,Abs) */ 
                              
	0xc0				      // END_COLLECTION
};

Joystick_::Joystick_()
{
	// Setup HID report structure
	static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
	HID().AppendDescriptor(&node);
}

void Joystick_::sendState()
{
	uint8_t data[JOYSTICK_STATE_SIZE];
	uint32_t buttonTmp = buttons;

	// Split 32 bit button-state into 4 bytes
	data[0] = buttonTmp & 0xFF;		
	buttonTmp >>= 8;
	data[1] = buttonTmp & 0xFF;
	buttonTmp >>= 8;
	data[2] = buttonTmp & 0xFF;
	buttonTmp >>= 8;
	data[3] = buttonTmp & 0xFF;

	data[4] = xAxis;
	data[5] = yAxis;

	// HID().SendReport(Report number, array of values in same order as HID descriptor, length)
	HID().SendReport(JOYSTICK_REPORT_ID, data, JOYSTICK_STATE_SIZE);
}

Joystick_ Joystick;

#endif
