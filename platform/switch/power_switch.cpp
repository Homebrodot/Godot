/**************************************************************************/
/*  power_switch.cpp                                                      */
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

#include "power_switch.h"

#include "switch_wrapper.h"

PowerSwitch::PowerSwitch() {
	if (R_SUCCEEDED(psmInitialize())) {
		initialized = true;
	}
}

OS::PowerState PowerSwitch::get_power_state() {
	if (!initialized) {
		return OS::POWERSTATE_UNKNOWN;
	}

	bool enough_power;
	psmIsEnoughPowerSupplied(&enough_power);

	if (!enough_power) {
		return OS::PowerState::POWERSTATE_ON_BATTERY;
	}

	int percentage = PowerSwitch::get_power_percent_left();

	if (percentage == 100) {
		return OS::PowerState::POWERSTATE_CHARGED;
	}

	return OS::PowerState::POWERSTATE_CHARGING;
}

int PowerSwitch::get_power_seconds_left() {
	WARN_PRINT("power_seconds_left is not implemented on this platform, defaulting to -1");
	return -1;
}

int PowerSwitch::get_power_percent_left() {
	if (!initialized) {
		return -1;
	}

	u32 voltage_percentage;
	psmGetBatteryChargePercentage(&voltage_percentage);
	return (int)voltage_percentage;
}
