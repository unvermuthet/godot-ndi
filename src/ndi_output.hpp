/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include "ndi.hpp"
#include "video_stream_ndi.hpp"

using namespace godot;

class NDIOutput : public Node {
	GDCLASS(NDIOutput, Node)

	protected:
		static void _bind_methods();

	public:
		NDIOutput();
		~NDIOutput();

		void _ready() override;
		void _exit_tree() override;
		void _process(double delta) override;

};
