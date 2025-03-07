/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "includes.hpp"

MissingNDIRuntimeDialog::MissingNDIRuntimeDialog() {
}

MissingNDIRuntimeDialog::~MissingNDIRuntimeDialog() {
}

void MissingNDIRuntimeDialog::open_download_link() {
	OS::get_singleton()->shell_open(String(NDILIB_REDIST_URL));
}

void MissingNDIRuntimeDialog::_bind_methods() {
}

void MissingNDIRuntimeDialog::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			dialog = memnew(ConfirmationDialog);

			dialog->set_title("NDI Runtime not found");
			dialog->set_text("Godot NDI requires the NDI Runtime to be installed on your system as well as on the system running the exported project. Check the verbose logs to see which paths were tried.");
			dialog->set_size(Vector2(530, 160));
			dialog->set_autowrap(true);

			dialog->set_ok_button_text("Download NDI Runtime");
			dialog->set_hide_on_ok(true);
			dialog->connect("confirmed", callable_mp_static(&MissingNDIRuntimeDialog::open_download_link));

			add_child(dialog);
		} break;

		case NOTIFICATION_READY: {
			if (ndi == nullptr) {
				dialog->popup_centered();
			}
		} break;
	}
}
