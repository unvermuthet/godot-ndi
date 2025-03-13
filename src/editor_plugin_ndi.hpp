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

class EditorPluginNDI : public EditorPlugin {
	GDCLASS(EditorPluginNDI, EditorPlugin)

public:
	EditorPluginNDI();
	~EditorPluginNDI();

protected:
	static void _bind_methods();
	void _notification(int p_what);
};
