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

class NDIOutput : public Node {
	GDCLASS(NDIOutput, Node)

	public:
		NDIOutput();
		~NDIOutput();

		void _enter_tree() override;
		void _exit_tree() override;
		void _process(double p_delta) override;

		void set_name(const String p_name);
		String get_name() const;

		void set_groups(const PackedStringArray p_groups);
		PackedStringArray get_groups() const;

	protected:
		static void _bind_methods();

	private:
		Ref<Image> mtx_img;

		Ref<Thread> thr;
		Ref<Mutex> mtx;
		Ref<Semaphore> sem;

		bool mtx_exit_thread = false;
		bool mtx_rebuild_send = false;

		String mtx_name;
		PackedStringArray mtx_groups;

		void process_thread();
};
