#include "core/config/project_settings.h"
#include "core/input/input.h"
#include "core/os/os.h"


#include "input/drc.h"
#include "input/kpad.h"


class OS_WiiU : public OS {
public: 

	InputWiiU *input_wiiu;

	enum TVResolution {
		RESOLUTION_1080P,
		RESOLUTION_720P,
		RESOLUTION_480P,
	}

	TVResolution tvres;

    //Most of this is copy pasted from the Windows platform code

	//I think I need to go back and redo my implementation of this
	virtual void alert(const String &p_alert, const String &p_title = "ALERT!") override;

	//Dunno what this is.
	virtual Error get_entropy(uint8_t *r_buffer, int p_bytes) override;

	//Maybe these will be used for RPLs? I'll have to look into what exactly these are for
	virtual Error open_dynamic_library(const String p_path, void *&p_library_handle, bool p_also_set_library_path = false, String *r_resolved_path = nullptr) override;
	virtual Error close_dynamic_library(void *p_library_handle) override;
	virtual Error get_dynamic_library_symbol_handle(void *p_library_handle, const String p_name, void *&p_symbol_handle, bool p_optional = false) override;

	virtual MainLoop *get_main_loop() const override;

	//OS info, some of this is implemented
	virtual String get_name() const override;
	virtual String get_distribution_name() const override;
	virtual String get_version() const override;
	virtual String get_locale() const override;


	virtual String get_executable_path() const override;

	//Hardware info
	virtual String get_processor_name() const override;
	virtual Vector<String> get_video_adapter_driver_info() const override;

	//To-do: Initialized WPADs, DRCs (which will be DisplayServer+VPAD)
	virtual void initialize_joypads() override {}

	//Time/date info. Should be easy, I just haven't put it in
	virtual DateTime get_datetime(bool p_utc) const override;
	virtual TimeZoneInfo get_time_zone_info() const override;
	virtual double get_unix_time() const override;

	//Font info
	virtual Vector<String> get_system_fonts() const override;
	virtual String get_system_font_path(const String &p_font_name, int p_weight = 400, int p_stretch = 100, bool p_italic = false) const override;
	virtual Vector<String> get_system_font_path_for_text(const String &p_font_name, const String &p_text, const String &p_locale = String(), const String &p_script = String(), int p_weight = 400, int p_stretch = 100, bool p_italic = false) const override;


	virtual Error set_cwd(const String &p_cwd) override;

	virtual void delay_usec(uint32_t p_usec) const override;
	virtual uint64_t get_ticks_usec() const override;

	virtual Dictionary get_memory_info() const override;

	virtual Error execute(const String &p_path, const List<String> &p_arguments, String *r_pipe = nullptr, int *r_exitcode = nullptr, bool read_stderr = false, Mutex *p_pipe_mutex = nullptr, bool p_open_console = false) override;
	virtual Error create_process(const String &p_path, const List<String> &p_arguments, ProcessID *r_child_id = nullptr, bool p_open_console = false) override;
	virtual Error kill(const ProcessID &p_pid) override;
	virtual int get_process_id() const override;
	virtual bool is_process_running(const ProcessID &p_pid) const override;

	//...What is an environment?
	virtual bool has_environment(const String &p_var) const override;
	virtual String get_environment(const String &p_var) const override;
	virtual void set_environment(const String &p_var, const String &p_value) const override;
	virtual void unset_environment(const String &p_var) const override;

	virtual uint64_t get_embedded_pck_offset() const override;

	virtual String get_config_path() const override;
	virtual String get_data_path() const override;
	virtual String get_cache_path() const override;
	virtual String get_godot_dir_name() const override;

	virtual String get_system_dir(SystemDir p_dir, bool p_shared_storage = true) const override;
	virtual String get_user_data_dir() const override;

	virtual String get_unique_id() const override;

	virtual Error shell_open(String p_uri) override;
	virtual Error shell_show_in_file_manager(String p_path, bool p_open_folder) override;

	//Basically the main loop for the platform *within* the engine
	void run();

	virtual bool _check_internal_feature_support(const String &p_feature) override;

	virtual void disable_crash_handler() override;
	virtual bool is_disable_crash_handler() const override;
	virtual void initialize_debugging() override;

	virtual Error move_to_trash(const String &p_path) override;

    OS_WiiU();
    ~OS_WiiU();
}

//
class WiiUTerminalLogger: public StdLogger {

}