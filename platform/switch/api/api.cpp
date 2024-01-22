/**************************************************************************/
/*  api.cpp                                                               */
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

#include "api.h"
#ifdef HORIZON_ENABLED
#include "os_switch.h"
#endif // HORIZON_ENABLED
#include "switch_singleton.h"

#include "core/engine.h"
#include "core/os/keyboard.h"

// === === ===
// === API ===
// === === ===

static NintendoSwitch *switch_eval;

void register_switch_api() {
	ClassDB::register_virtual_class<NintendoSwitch>();
	switch_eval = memnew(NintendoSwitch);
	Engine::get_singleton()->add_singleton(Engine::Singleton("NintendoSwitch", switch_eval));
}

void unregister_switch_api() {
	memdelete(switch_eval);
}

// === === === === === === === === ===
// ===  Nintendo Switch Singleton  ===
// === === === === === === === === ===

bool g_swkbd_open = false;

NintendoSwitch *NintendoSwitch::singleton = nullptr;

NintendoSwitch *NintendoSwitch::get_singleton() {
	return singleton;
}

NintendoSwitch::NintendoSwitch() {
	ERR_FAIL_COND_MSG(singleton != nullptr, "NintendoSwitch singleton already exists.");
	singleton = this;
#ifdef HORIZON_ENABLED
	swkbdInlineCreate(&inline_keyboard);
#endif // HORIZON_ENABLED
}

void NintendoSwitch::_bind_methods() {
	ClassDB::bind_method(D_METHOD("show_virtual_keyboard", "existing_text", "type"), &NintendoSwitch::show_virtual_keyboard, DEFVAL(""), DEFVAL(NORMAL_KEYBOARD));

	BIND_ENUM_CONSTANT(NORMAL_KEYBOARD)
	BIND_ENUM_CONSTANT(NUMPAD_KEYBOARD)
	BIND_ENUM_CONSTANT(QWERTY_KEYBOARD)
	BIND_ENUM_CONSTANT(LATIN_KEYBOARD)
	BIND_ENUM_CONSTANT(SIMPLIFIED_CHINESE_KEYBOARD)
	BIND_ENUM_CONSTANT(TRADITIONAL_CHINESE_KEYBOARD)
	BIND_ENUM_CONSTANT(KOREAN_KEYBOARD)
	BIND_ENUM_CONSTANT(ALL_LANGUAGES_KEYBOARD)
}

#ifdef HORIZON_ENABLED
int g_eat_string_events = 0;

u32 last_length = 0;
void keyboard_string_changed_callback(const char *str, SwkbdChangedStringArg *arg) {
	// Adjusted for NUL-terminator
	u32 string_length = arg->stringLen + 3;

	// We get a string changed event on appear, and another one on setting text.
	if (g_eat_string_events) {
		last_length = string_length;
		g_eat_string_events--;
		return;
	}

	if (string_length < last_length) {
		OS_Switch::get_singleton()->key(KEY_BACKSPACE, true);
	} else if (string_length > 0) {
		OS_Switch::get_singleton()->key(str[string_length - 1], true);
	}
	last_length = string_length;
}

int last_cursor = 0;
void keyboard_moved_cursor_callback(const char *str, SwkbdMovedCursorArg *arg) {
	if (arg->cursorPos < last_cursor) {
		OS_Switch::get_singleton()->key(KEY_LEFT, true);
	} else {
		OS_Switch::get_singleton()->key(KEY_RIGHT, true);
	}

	last_cursor = arg->cursorPos;
}

void keyboard_decided_enter_callback(const char *str, SwkbdDecidedEnterArg *arg) {
	OS_Switch::get_singleton()->key(KEY_ENTER, true);

	g_swkbd_open = false;
}

void keyboard_decided_cancel_callback() {
	g_swkbd_open = false;
}
#endif // HORIZON_ENABLED

void NintendoSwitch::initialize_software_keyboard() {
#ifdef HORIZON_ENABLED
	swkbdInlineLaunchForLibraryApplet(&inline_keyboard, SwkbdInlineMode_AppletDisplay, 0);
	swkbdInlineSetChangedStringCallback(&inline_keyboard, keyboard_string_changed_callback);
	swkbdInlineSetMovedCursorCallback(&inline_keyboard, keyboard_moved_cursor_callback);
	swkbdInlineSetDecidedEnterCallback(&inline_keyboard, keyboard_decided_enter_callback);
	swkbdInlineSetDecidedCancelCallback(&inline_keyboard, keyboard_decided_cancel_callback);
#endif // HORIZON_ENABLED
}

void NintendoSwitch::update() {
#ifdef HORIZON_ENABLED
	swkbdInlineUpdate(&inline_keyboard, NULL);
#endif // HORIZON_ENABLED
}

void NintendoSwitch::show_virtual_keyboard(const String &p_existing_text, SoftwareKeyboardType p_type) {
#ifdef HORIZON_ENABLED
	if (!g_swkbd_open) {
		SwkbdAppearArg appear_arg;
		swkbdInlineMakeAppearArg(&appear_arg, (SwkbdType)p_type);
		swkbdInlineSetInputText(&inline_keyboard, p_existing_text.utf8().get_data());
		swkbdInlineSetCursorPos(&inline_keyboard, p_existing_text.size() - 1);

		swkbdInlineAppear(&inline_keyboard, &appear_arg);

		g_eat_string_events = 2;
		g_swkbd_open = true;
	}
#endif // HORIZON_ENABLED
}

void NintendoSwitch::hide_virtual_keyboard() {
#ifdef HORIZON_ENABLED
	swkbdInlineDisappear(&inline_keyboard);
	g_swkbd_open = false;
#endif // HORIZON_ENABLED
}

bool NintendoSwitch::is_virtual_keyboard_open() {
	return g_swkbd_open;
}

void NintendoSwitch::cleanup() {
#ifdef HORIZON_ENABLED
	swkbdInlineClose(&inline_keyboard);
#endif // HORIZON_ENABLED
}
