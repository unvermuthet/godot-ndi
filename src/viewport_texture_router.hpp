/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <godot_cpp/classes/rd_texture_format.hpp>
#include <godot_cpp/classes/viewport.hpp>

using namespace godot;

class ViewportTextureRouter : public Object {
	GDCLASS(ViewportTextureRouter, Object)

public:
	ViewportTextureRouter();
	~ViewportTextureRouter();

	void add_viewport(Viewport *viewport);
	void remove_viewport(Viewport *viewport);

protected:
	static void _bind_methods();

private:
	Dictionary vps;
	void request_textures();
	void forward_texture(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format, int64_t p_viewport_rid);
};
