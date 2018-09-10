/**************************************************************************/
/*  joypad_switch.cpp                                                     */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "joypad_switch.h"

static u64 pad_ids[JOYPADS_MAX] = {
	(1ull << HidNpadIdType_No1) | (1ull << HidNpadIdType_Handheld),
	(1ull << HidNpadIdType_No2),
	(1ull << HidNpadIdType_No3),
	(1ull << HidNpadIdType_No4),
	(1ull << HidNpadIdType_No5),
	(1ull << HidNpadIdType_No6),
	(1ull << HidNpadIdType_No7),
	(1ull << HidNpadIdType_No8)
};

// from editor "Project Settings > Input Map"
static const HidNpadButton pad_mapping[] = {
	HidNpadButton_B, HidNpadButton_A, HidNpadButton_Y, HidNpadButton_X,
	HidNpadButton_L, HidNpadButton_R, HidNpadButton_ZL, HidNpadButton_ZR,
	HidNpadButton_StickL, HidNpadButton_StickR,
	HidNpadButton_Minus, HidNpadButton_Plus,
	HidNpadButton_Up, HidNpadButton_Down, HidNpadButton_Left, HidNpadButton_Right
};

JoypadSwitch::JoypadSwitch(InputDefault *in) {
	input = in;

	// TODO: n players?
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);

	button_count = sizeof(pad_mapping) / sizeof(*pad_mapping);
	for (int i = 0; i < JOYPADS_MAX; i++) {
		padInitializeWithMask(&pads[i], pad_ids[i]);
	}
}

JoypadSwitch::~JoypadSwitch() {
}

void JoypadSwitch::process() {
	for (int index = 0; index < JOYPADS_MAX; index++) {
		padUpdate(&pads[index]);

		HidAnalogStickState l_stick = padGetStickPos(&pads[index], 0);
		HidAnalogStickState r_stick = padGetStickPos(&pads[index], 1);

		// Axes
		input->joy_axis(index, 0, (float)(l_stick.x / 32767.0f));
		input->joy_axis(index, 1, (float)(-l_stick.y / 32767.0f));
		input->joy_axis(index, 2, (float)(r_stick.x / 32767.0f));
		input->joy_axis(index, 3, (float)(-r_stick.y / 32767.0f));

		// Buttons
		u64 buttons_up = padGetButtonsUp(&pads[index]);
		u64 buttons_down = padGetButtonsDown(&pads[index]);

		if (buttons_up != 0 || buttons_down != 0) {
			for (int i = 0; i < button_count; i++) {
				if (buttons_up & pad_mapping[i]) {
					input->joy_button(index, i, false);
				}
				if (buttons_down & pad_mapping[i]) {
					input->joy_button(index, i, true);
				}
			}
		}
	}
}
