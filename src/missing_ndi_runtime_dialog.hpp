/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "includes.hpp"

using namespace godot;

class MissingNDIRuntimeDialog : public EditorPlugin {
	GDCLASS(MissingNDIRuntimeDialog, EditorPlugin)

public:
	MissingNDIRuntimeDialog();
	~MissingNDIRuntimeDialog();
	static void open_download_link();

protected:
	static void _bind_methods();
	void _notification(int p_what);

private:
	ConfirmationDialog *dialog;
};
