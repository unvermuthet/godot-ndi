/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ndi_runtime_dialog.hpp"

#include "ndi.hpp"

#include <godot_cpp/classes/os.hpp>

using namespace godot;

NDIRuntimeDialog::NDIRuntimeDialog() {
	set_title("NDI Runtime not found");
	set_text("Godot NDI requires the NDI Runtime to be installed on your system. Check the verbose logs to see which paths were tried.");
	set_size(Vector2(530, 160));
	set_autowrap(true);

	set_ok_button_text("Download NDI Runtime");
	set_hide_on_ok(true);
	connect("confirmed", callable_mp_static(&NDIRuntimeDialog::open_download_link));
}

NDIRuntimeDialog::~NDIRuntimeDialog() {
}

String NDIRuntimeDialog::get_download_link() {
	return String(NDILIB_REDIST_URL);
}

void NDIRuntimeDialog::open_download_link() {
	OS::get_singleton()->shell_open(get_download_link());
}

void NDIRuntimeDialog::_bind_methods() {
	ClassDB::bind_static_method("NDIRuntimeDialog", D_METHOD("get_download_link"), &NDIRuntimeDialog::get_download_link);
	ClassDB::bind_static_method("NDIRuntimeDialog", D_METHOD("open_download_link"), &NDIRuntimeDialog::open_download_link);
}

void NDIRuntimeDialog::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			if (ndi == nullptr) {
				popup_centered();
			}
		} break;
	}
}
