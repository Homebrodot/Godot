/**************************************************************************/
/*  export.cpp                                                            */
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

// XXX: fix your windows build with this one weird trick
// (Microsoft doesn't like strncpy..)
#define _CRT_SECURE_NO_WARNINGS

#include "export.h"

#include "core/io/packet_peer_udp.h"
#include "core/os/file_access.h"
#include "editor/editor_export.h"
#include "editor/editor_node.h"
#include "platform/switch/logo.gen.h"
#include "scene/resources/texture.h"

#define TEMPLATE_RELEASE "switch_release.nro"
#define TEMPLATE_APPLET_SPLASH "switch_applet_splash.rgba.gz"

class ExportPluginSwitch : public EditorExportPlugin {
public:
	Vector<uint8_t> editor_id_vec;

protected:
	virtual void _export_begin(const Set<String> &p_features, bool p_debug, const String &p_path, int p_flags) {
		if (editor_id_vec.size() != 0) {
			add_file("custom_editor_id", editor_id_vec, false);
		}
	}
};

class EditorExportPlatformSwitch : public EditorExportPlatform {
	GDCLASS(EditorExportPlatformSwitch, EditorExportPlatform)

	Ref<ImageTexture> logo;

	Vector<String> devices;
	volatile bool devices_changed;
	Mutex device_lock;
	Thread device_thread;
	volatile bool quit_request;

	ExportPluginSwitch *export_plugin;

	static void _device_poll_thread(void *ud) {
		EditorExportPlatformSwitch *ea = (EditorExportPlatformSwitch *)ud;

		PacketPeerUDP peer;
		if (peer.listen(28771) != OK) {
			// ???
		}

		peer.set_broadcast_enabled(true);
		peer.set_dest_address(IP_Address("255.255.255.255"), 28280);

		const uint8_t *packet_buff;
		int packet_len;

		while (!ea->quit_request) {
			bool different = false;

			peer.put_packet((unsigned char *)"nxboot", strlen("nxboot"));
			OS::get_singleton()->delay_usec(500000); // delay 500ms to allow for actual replies

			Vector<String> ndevices;

			while (peer.get_packet(&packet_buff, packet_len) != ERR_UNAVAILABLE) {
				IP_Address a = peer.get_packet_address();
				ndevices.push_back(String(a));
			}

			ndevices.sort();

			ea->device_lock.lock();

			if (ndevices.size() != ea->devices.size()) {
				different = true;
			} else {
				for (int i = 0; i < ea->devices.size(); i++) {
					if (ea->devices[i] != ndevices[i]) {
						different = true;
						break;
					}
				}
			}

			if (different) {
				ea->devices = ndevices;
				ea->devices_changed = true;
			}

			ea->device_lock.unlock();

			uint64_t sleep = OS::get_singleton()->get_power_state() == OS::POWERSTATE_ON_BATTERY ? 1000 : 100;
			uint64_t wait = 3000000;
			uint64_t time = OS::get_singleton()->get_ticks_usec();
			while (OS::get_singleton()->get_ticks_usec() - time < wait) {
				OS::get_singleton()->delay_usec(1000 * sleep);
				if (ea->quit_request)
					break;
			}
		}
	}

public:
	virtual void get_preset_features(const Ref<EditorExportPreset> &p_preset, List<String> *r_features) {
		String driver = ProjectSettings::get_singleton()->get("rendering/quality/driver/driver_name");
		if (driver == "GLES2") {
			r_features->push_back("etc");
		} else if (driver == "GLES3") {
			r_features->push_back("etc2");
			if (ProjectSettings::get_singleton()->get("rendering/quality/driver/fallback_to_gles2")) {
				r_features->push_back("etc");
			}
		}
	}

	virtual void get_platform_features(List<String> *r_features) {
		r_features->push_back("mobile");
	}

	virtual void get_export_options(List<ExportOption> *r_options) {
		String title = ProjectSettings::get_singleton()->get("application/config/name");
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "application/fused_build"), false));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/custom_editor_id"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/title", PROPERTY_HINT_PLACEHOLDER_TEXT, title), title));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/author", PROPERTY_HINT_PLACEHOLDER_TEXT, "Game Author"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/version", PROPERTY_HINT_PLACEHOLDER_TEXT, "Game Version"), "1.0"));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/icon_256x256", PROPERTY_HINT_GLOBAL_FILE, "*.jpg"), ""));
	}

	virtual String get_name() const {
		return "Nintendo Switch";
	}

	virtual String get_os_name() const {
		return "Switch";
	}

	virtual Ref<Texture> get_logo() const {
		return logo;
	}

	virtual Ref<Texture> get_run_icon() const {
		return logo;
	}

	virtual bool poll_export() {
		bool dc = devices_changed;
		if (dc) {
			// don't clear unless we're reporting true, to avoid race
			devices_changed = false;
		}
		return dc;
	}

	virtual int get_options_count() const {
		device_lock.lock();
		int dc = devices.size();
		device_lock.unlock();

		return dc;
	}

	virtual String get_options_tooltip() const {
		return TTR("Select device from the list");
	}

	virtual String get_option_label(int p_index) const {
		ERR_FAIL_INDEX_V(p_index, devices.size(), "");
		device_lock.lock();
		String s = devices[p_index];
		device_lock.unlock();
		return s;
	}

	virtual String get_option_tooltip(int p_index) const {
		ERR_FAIL_INDEX_V(p_index, devices.size(), "");
		device_lock.lock();
		String s = devices[p_index];
		device_lock.unlock();
		return s;
	}
	virtual Error run(const Ref<EditorExportPreset> &p_preset, int p_device, int p_debug_flags) {
		String can_export_error;
		bool can_export_missing_templates;
		if (!can_export(p_preset, can_export_error, can_export_missing_templates)) {
			EditorNode::add_io_error(can_export_error);
			return ERR_UNCONFIGURED;
		}

		EditorProgress ep("run", "Running", 2);

		print_line("Exporting...");
		if (ep.step("Exporting...", 0)) {
			return ERR_SKIP;
		}

		String tmp_pack_path = EditorSettings::get_singleton()->get_cache_dir().plus_file("tmpexport.pck");

		Error err = save_pack(p_preset, tmp_pack_path);

		if (err != OK) {
			DirAccess::remove_file_or_error(tmp_pack_path);
			return err;
		}

		print_line("Sending...");
		if (ep.step("Sending...", 1)) {
			DirAccess::remove_file_or_error(tmp_pack_path);
			return err;
		}

		String nxlink = EditorSettings::get_singleton()->get("export/switch/nxlink");
		// If we can't find it, look for a bundled copy.
		if (nxlink == "") {
			String exe_ext;
			if (OS::get_singleton()->get_name() == "Windows") {
				exe_ext = ".exe";
			}
			nxlink = OS::get_singleton()->get_executable_path().get_base_dir() + "/nxlink" + exe_ext;
		}

		if (FileAccess::exists(nxlink)) {
			List<String> args;
			int ec;

			args.push_back(tmp_pack_path);
			args.push_back("-a");
			args.push_back(devices[p_device]);
			args.push_back("-p");
			args.push_back("TempExport.pck");
			args.push_back("--args");

			Vector<String> inner_args;
			// todo: editor arg
			inner_args.push_back("-v");

			gen_export_flags(inner_args, p_debug_flags);
			args.push_back(String(" ").join(inner_args));

			OS::get_singleton()->execute(nxlink, args, true, NULL, NULL, &ec);
		} else {
			EditorNode::get_singleton()->show_warning(TTR("nxlink binary not found! Set its path in Editor Settings."));
		}

		DirAccess::remove_file_or_error(tmp_pack_path);
		return OK;
	}

	virtual bool can_export(const Ref<EditorExportPreset> &p_preset, String &r_error, bool &r_missing_templates) const {
		String err;
		r_missing_templates =
				find_export_template(TEMPLATE_RELEASE) == String() ||
				find_export_template(TEMPLATE_APPLET_SPLASH) == String();

		bool valid = !r_missing_templates;
		String etc_error = test_etc2();
		if (etc_error != String()) {
			err += etc_error;
			valid = false;
		}

		if (!err.empty()) {
			r_error = err;
		}

		return valid;
	}

	virtual List<String> get_binary_extensions(const Ref<EditorExportPreset> &p_preset) const {
		List<String> list;
		list.push_back("nro");
		return list;
	}

	virtual void resolve_platform_feature_priorities(const Ref<EditorExportPreset> &p_preset, Set<String> &p_features) {
	}

	virtual Error export_pack(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags = 0) {
		// XXX i hate this - we have to do this _before_ the export notifier
		String custom_editor_id = p_preset->get("application/custom_editor_id");
		export_plugin->editor_id_vec.clear();

		if (custom_editor_id != "") {
			// XXX what the hell is this
			const char *chars = custom_editor_id.utf8().ptr();
			for (size_t i = 0; i < strlen(chars); i++) {
				export_plugin->editor_id_vec.push_back(chars[i]);
			}
		}

		ExportNotifier notifier(*this, p_preset, p_debug, p_path, p_flags);

		return save_pack(p_preset, p_path);
	}

	virtual Error export_project(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags = 0) {
		// XXX i hate this - we have to do this _before_ the export notifier
		String custom_editor_id = p_preset->get("application/custom_editor_id");
		export_plugin->editor_id_vec.clear();

		if (custom_editor_id != "") {
			// XXX what the hell is this
			const char *chars = custom_editor_id.utf8().ptr();
			for (size_t i = 0; i < strlen(chars); i++) {
				export_plugin->editor_id_vec.push_back(chars[i]);
			}
		}

		ExportNotifier notifier(*this, p_preset, p_debug, p_path, p_flags);

		if (!DirAccess::exists(p_path.get_base_dir())) {
			return ERR_FILE_BAD_PATH;
		}

		String nro_path = find_export_template(TEMPLATE_RELEASE);
		if (nro_path != String() && !FileAccess::exists(nro_path)) {
			EditorNode::get_singleton()->show_warning(TTR("Template file not found:") + "\n" + nro_path);
			return ERR_FILE_NOT_FOUND;
		}

		DirAccess *da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);

		Error err;
		// update nro icon/title/author/version
		String title = p_preset->get("application/title");
		String author = p_preset->get("application/author");
		String version = p_preset->get("application/version");
		String icon = p_preset->get("application/icon_256x256");

		NacpStruct *nacp = memnew(NacpStruct);
		memset(nacp, 0, sizeof(NacpStruct));
		create_nacp(nacp, title, author, version);

		if (p_preset->get("application/fused_build")) {
			String build_romfs = EditorSettings::get_singleton()->get("export/switch/build_romfs");

			// If we can't find it, look for a bundled copy.
			if (build_romfs == "") {
				String exe_ext;
				if (OS::get_singleton()->get_name() == "Windows") {
					exe_ext = ".exe";
				}
				build_romfs = OS::get_singleton()->get_executable_path().get_base_dir() + "/build_romfs" + exe_ext;
			}

			if (build_romfs != String() && FileAccess::exists(build_romfs)) {
				String cache = EditorSettings::get_singleton()->get_cache_dir();
				String romfs_dir = cache.plus_file("romfs");
				String romfs_bin_path = cache.plus_file("romfs.bin");

				da->make_dir(romfs_dir);
				err = save_pack(p_preset, romfs_dir.plus_file("game.pck"));
				if (err == OK) {
					String applet_splash = find_export_template(TEMPLATE_APPLET_SPLASH);
					if (FileAccess::exists(applet_splash)) {
						da->copy(applet_splash, romfs_dir.plus_file("applet_splash.rgba.gz"));

						List<String> args;
						int ec;
						args.push_back(romfs_dir);
						args.push_back(romfs_bin_path);
						OS::get_singleton()->execute(build_romfs, args, true, NULL, NULL, &ec);
						if (ec == 0) {
							err = create_nro(nro_path, p_path, nacp, icon, romfs_bin_path);
							DirAccess::remove_file_or_error(romfs_bin_path);
						} else {
							EditorNode::get_singleton()->show_warning(TTR("build_romfs failed!"));
							err = ERR_BUG;
						}
					} else {
						EditorNode::get_singleton()->show_warning(TTR("Template file not found:") + "\n" + applet_splash);
						err = ERR_FILE_NOT_FOUND;
					}
				} else {
					EditorNode::get_singleton()->show_warning(TTR("Pack export failed!"));
				}

				DirAccess::remove_file_or_error(romfs_dir);
			} else {
				EditorNode::get_singleton()->show_warning(TTR("build_romfs binary not found! Set its path in Editor Settings."));
				err = ERR_FILE_NOT_FOUND;
			}
		} else {
			String empty_string = "";

			err = save_pack(p_preset, p_path.get_basename() + ".pck");
			if (err == OK) {
				err = create_nro(nro_path, p_path, nacp, icon, empty_string);
			} else {
				EditorNode::get_singleton()->show_warning(TTR("Pack export failed!"));
			}
		}

		memdelete(nacp);
		memdelete(da);
		return err;
	}

	void copy_chunked(FileAccess *src, FileAccess *dst, size_t len, size_t chunk_size = 0x100000) {
		uint8_t *buffer = (uint8_t *)malloc(chunk_size);
		while (len > chunk_size) {
			size_t amt = src->get_buffer(buffer, chunk_size);
			dst->store_buffer(buffer, amt);
			len -= amt;
		}

		src->get_buffer(buffer, len);
		dst->store_buffer(buffer, len);

		free(buffer);
	}

	Error create_nro(const String &template_path, const String &output_path, NacpStruct *nacp, String &icon_path, String &romfs_path) {
		NroHeader nro_header;
		NroAssetHeader asset_header;

		FileAccess *template_f = FileAccess::open(template_path, FileAccess::READ);
		FileAccess *nro = FileAccess::open(output_path, FileAccess::WRITE);

		if (!template_f || !nro)
			return ERR_FILE_NOT_FOUND;

		template_f->seek(sizeof(NroStart));
		template_f->get_buffer((uint8_t *)&nro_header, sizeof(NroHeader));

		if (memcmp(&nro_header.magic, "NRO0", 4) != 0) {
			template_f->close();
			memdelete(template_f);
			nro->close();
			memdelete(nro);
			return ERR_INVALID_DATA;
		}

		// copy nro body to output
		template_f->seek(0);
		copy_chunked(template_f, nro, nro_header.size);

		template_f->seek(nro_header.size);
		template_f->get_buffer((uint8_t *)&asset_header, sizeof(NroAssetHeader));
		if (memcmp(&asset_header.magic, "ASET", 4) != 0) {
			template_f->close();
			memdelete(template_f);
			nro->close();
			memdelete(nro);
			return ERR_INVALID_DATA;
		}

		NroAssetHeader new_asset_header;
		memset(&new_asset_header, 0, sizeof(NroAssetHeader));

		// write dummy asset header here
		nro->store_buffer((uint8_t *)&new_asset_header, sizeof(NroAssetHeader));

		memcpy(&new_asset_header.magic, "ASET", 4);
		new_asset_header.version = 0;
		new_asset_header.icon.offset = nro->get_position() - nro_header.size;

		if (icon_path != String() && FileAccess::exists(icon_path)) {
			// replace icon
			FileAccess *icon = FileAccess::open(icon_path, FileAccess::READ);
			size_t icon_len = icon->get_len();
			copy_chunked(icon, nro, icon_len);

			new_asset_header.icon.size = icon_len;
			icon->close();
			memdelete(icon);
		} else {
			// copy icon
			template_f->seek(nro_header.size + asset_header.icon.offset);
			copy_chunked(template_f, nro, asset_header.icon.size);
			new_asset_header.icon.size = asset_header.icon.size;
		}

		// write new nacp
		new_asset_header.nacp.offset = nro->get_position() - nro_header.size;
		new_asset_header.nacp.size = sizeof(NacpStruct);
		nro->store_buffer((uint8_t *)nacp, sizeof(NacpStruct));

		new_asset_header.romfs.offset = nro->get_position() - nro_header.size;

		if (romfs_path != String() && FileAccess::exists(romfs_path)) {
			FileAccess *romfs = FileAccess::open(romfs_path, FileAccess::READ);
			size_t romfs_len = romfs->get_len();
			copy_chunked(romfs, nro, romfs_len);

			new_asset_header.romfs.size = romfs_len;
			romfs->close();
			memdelete(romfs);
		} else {
			template_f->seek(nro_header.size + asset_header.romfs.offset);
			copy_chunked(template_f, nro, asset_header.romfs.size);

			new_asset_header.romfs.size = asset_header.romfs.size;
		}

		// Go back and actually write the asset header
		nro->seek(nro_header.size);
		nro->store_buffer((uint8_t *)&new_asset_header, sizeof(NroAssetHeader));

		template_f->close();
		memdelete(template_f);
		nro->close();
		memdelete(nro);

		return OK;
	}

	void create_nacp(NacpStruct *nacp, String &title, String &author, String &version) {
		CharString title_utf8 = title.utf8();
		CharString author_utf8 = author.utf8();
		CharString version_utf8 = version.utf8();

		const char *title_cstr = title_utf8.ptr();
		const char *author_cstr = author_utf8.ptr();
		const char *version_cstr = version_utf8.ptr();

		for (int i = 0; i < 12; i++) {
			if (title.length() != 0) {
				strncpy(nacp->lang[i].name, title_cstr, sizeof(nacp->lang[i].name) - 1);
			}
			if (author.length() != 0) {
				strncpy(nacp->lang[i].author, author_cstr, sizeof(nacp->lang[i].author) - 1);
			}
		}

		if (version.length() != 0) {
			strncpy(nacp->display_version, version_cstr, sizeof(nacp->display_version) - 1);
		}
	}

	EditorExportPlatformSwitch() {
		Ref<Image> img = memnew(Image(_switch_logo));
		logo.instance();
		logo->create_from_image(img);

		devices_changed = true;
		quit_request = false;
		device_thread.start(_device_poll_thread, this);

		export_plugin = memnew(ExportPluginSwitch);
		EditorExport::get_singleton()->add_export_plugin(export_plugin);
	}

	~EditorExportPlatformSwitch() {
		quit_request = true;
		device_thread.wait_to_finish();

		// DO NOT free it
		//memdelete(export_plugin);
	}
};

void register_switch_exporter() {
	String exe_ext;
	if (OS::get_singleton()->get_name() == "Windows") {
		exe_ext = "*.exe";
	}

	EDITOR_DEF("export/switch/nxlink", "");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, "export/switch/nxlink", PROPERTY_HINT_GLOBAL_FILE, exe_ext));

	EDITOR_DEF("export/switch/build_romfs", "");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, "export/switch/build_romfs", PROPERTY_HINT_GLOBAL_FILE, exe_ext));

	Ref<EditorExportPlatformSwitch> exporter = Ref<EditorExportPlatformSwitch>(memnew(EditorExportPlatformSwitch));
	EditorExport::get_singleton()->add_export_platform(exporter);
}
