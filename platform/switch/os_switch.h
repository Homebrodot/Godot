/**************************************************************************/
/*  os_switch.h                                                           */
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

#include "audio_driver_switch.h"
#include "context_gl_switch_egl.h"
#include "core/os/input.h"
#include "core/os/os.h"
#include "joypad_switch.h"
#include "main/input_default.h"
#include "power_switch.h"
#include "servers/visual/visual_server_raster.h"

class OS_Switch : public OS {
	int video_driver_index;
	MainLoop *main_loop;
	VideoMode current_videomode;
	VisualServer *visual_server;
	InputDefault *input;
	PowerSwitch *power_manager;
	ContextGLSwitchEGL *gl_context;
	JoypadSwitch *joypad;
	AudioDriverSwitch driver_switch;
	String switch_execpath;

	SwkbdInline inline_keyboard;

protected:
	virtual void initialize_core();
	virtual Error initialize(const VideoMode &p_desired, int p_video_driver, int p_audio_driver);

	virtual void set_main_loop(MainLoop *p_main_loop);
	virtual void delete_main_loop();

	virtual void finalize();
	virtual void finalize_core();

public:
	virtual bool _check_internal_feature_support(const String &p_feature);

	virtual void alert(const String &p_alert, const String &p_title = "ALERT!");
	virtual String get_stdin_string(bool p_block = true);
	virtual Point2 get_mouse_position() const;
	virtual int get_mouse_button_state() const;
	virtual void set_window_title(const String &p_title);

	virtual void set_video_mode(const VideoMode &p_video_mode, int p_screen);
	virtual VideoMode get_video_mode(int p_screen) const;
	virtual void get_fullscreen_mode_list(List<VideoMode> *p_list, int p_screen) const;
	OS::RenderThreadMode get_render_thread_mode() const;

	virtual int get_current_video_driver() const;
	virtual Size2 get_window_size() const;

	virtual Error execute(const String &p_path, const List<String> &p_arguments, bool p_blocking, ProcessID *r_child_id = NULL, String *r_pipe = NULL, int *r_exitcode = NULL, bool read_stderr = false, Mutex *p_pipe_mutex = NULL, bool p_open_console = false);
	virtual Error kill(const ProcessID &p_pid);
	virtual bool is_process_running(const ProcessID &p_pid) const;

	virtual String get_executable_path() const;
	virtual void set_executable_path(const char *p_execpath);
	virtual String get_user_data_dir() const;
	virtual String get_data_path() const;

	virtual bool has_environment(const String &p_var) const;
	virtual String get_environment(const String &p_var) const;
	virtual bool set_environment(const String &p_var, const String &p_value) const;
	virtual String get_name() const;
	virtual MainLoop *get_main_loop() const;
	virtual Date get_date(bool local = false) const;
	virtual Time get_time(bool local = false) const;
	virtual TimeZoneInfo get_time_zone_info() const;
	virtual void delay_usec(uint32_t p_usec) const;
	virtual uint64_t get_ticks_usec() const;
	virtual bool can_draw() const;
	virtual void set_cursor_shape(CursorShape p_shape);
	virtual void set_custom_mouse_cursor(const RES &p_cursor, CursorShape p_shape, const Vector2 &p_hotspot);

	OS::PowerState get_power_state();
	int get_power_seconds_left();
	int get_power_percent_left();

	virtual bool has_touchscreen_ui_hint() const;

	virtual bool has_virtual_keyboard() const;
	virtual void show_virtual_keyboard(const String &p_existing_text, const Rect2 &p_screen_rect = Rect2(), int p_max_input_length = -1);
	virtual void hide_virtual_keyboard();
	virtual int get_virtual_keyboard_height() const;

	void run();
	virtual void swap_buffers();

	void key(uint32_t p_key, bool p_pressed);

	static OS_Switch *get_singleton();

	OS_Switch();
};
