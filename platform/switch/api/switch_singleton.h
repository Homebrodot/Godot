/**************************************************************************/
/*  switch_singleton.h                                                    */
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

#ifndef SWITCH_SINGLETON_H
#define SWITCH_SINGLETON_H

#ifndef MODULE_MONO_ENABLED

#include "core/object.h"
#include "core/variant.h"
#ifdef HORIZON_ENABLED
#include "switch_wrapper.h"
#endif // HORIZON_ENABLED

class NintendoSwitch : public Object {
	GDCLASS(NintendoSwitch, Object);

public:
	enum SoftwareKeyboardType {
		NORMAL_KEYBOARD = 0,
		NUMPAD_KEYBOARD = 1,
		QWERTY_KEYBOARD = 2,
		LATIN_KEYBOARD = 4,
		SIMPLIFIED_CHINESE_KEYBOARD = 5,
		TRADITIONAL_CHINESE_KEYBOARD = 6,
		KOREAN_KEYBOARD = 7,
		ALL_LANGUAGES_KEYBOARD = 8,
	};

	int g_eat_string_events = 0;
#ifdef HORIZON_ENABLED
	SwkbdInline inline_keyboard;
#endif // HORIZON_ENABLED

protected:
	static NintendoSwitch *singleton;

	static void _bind_methods();

public:
	static NintendoSwitch *get_singleton();

	void initialize_software_keyboard();
	void update();
	void show_virtual_keyboard(const String &p_existing_text, SoftwareKeyboardType p_type);
	void hide_virtual_keyboard();
	bool is_virtual_keyboard_open();

	void cleanup();

	NintendoSwitch();
};

VARIANT_ENUM_CAST(NintendoSwitch::SoftwareKeyboardType);

#endif // MODULE_MONO_ENABLED

#endif // SWITCH_SINGLETON_H
