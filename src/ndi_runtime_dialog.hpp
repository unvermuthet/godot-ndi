/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <godot_cpp/classes/confirmation_dialog.hpp>

using namespace godot;

class NDIRuntimeDialog : public ConfirmationDialog {
	GDCLASS(NDIRuntimeDialog, ConfirmationDialog)

public:
	NDIRuntimeDialog();
	~NDIRuntimeDialog();
	static void open_download_link();
	static String get_download_link();

protected:
	static void _bind_methods();
	void _notification(int p_what);
};
