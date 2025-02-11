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

class ViewportTextureRouter : public Node {
	GDCLASS(ViewportTextureRouter, Node)

public:
	ViewportTextureRouter();
	~ViewportTextureRouter();

	void add_viewport(Viewport *viewport);
	void remove_viewport(Viewport *viewport);

protected:
	static void _bind_methods();

private:
	TypedArray<Viewport> vps;
	void request_textures();
	void forward_texture(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format, Viewport *p_viewport);
};