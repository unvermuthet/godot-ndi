/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ndi_version_check.hpp"

#include "ndi.hpp"

#include <godot_cpp/classes/json.hpp>

using namespace godot;

NDIVersionCheck::NDIVersionCheck() {
}

NDIVersionCheck::~NDIVersionCheck() {
}

Dictionary NDIVersionCheck::get_copyright_info() {
	auto infos = Dictionary();
	infos.set("name", "Godot NDI");

	{
		auto parts = Array();
		{
			auto part = Dictionary();

			auto files = Array();
			files.append("godot-ndi/src/*");

			auto copyright = Array();
			copyright.append("Copyright (C) 2025-present Henry Muth - unvermuthet");

			part.set("files", files);
			part.set("copyright", copyright);
			part.set("license", "MPL-2.0");
			parts.append(part);
		}
		{
			auto part = Dictionary();

			auto files = Array();
			files.append("godot-ndi/ndi/*.h");

			auto copyright = Array();
			copyright.append("Copyright (C) 2023-2024 Vizrt NDI AB. All rights reserved.");

			part.set("files", files);
			part.set("copyright", copyright);
			part.set("license", "MIT");
			parts.append(part);
		}
		infos.set("parts", parts);
	}

	return infos;
}
void NDIVersionCheck::_bind_methods() {
	ClassDB::bind_static_method("NDIVersionCheck", D_METHOD("get_copyright_info"), &NDIVersionCheck::get_copyright_info);
}

void NDIVersionCheck::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			connect("request_completed", callable_mp(this, &NDIVersionCheck::_on_request_completed));
			request("https://api.github.com/repos/unvermuthet/godot-ndi/releases/latest");
		} break;
	}
}

void NDIVersionCheck::_on_request_completed(HTTPRequest::Result p_result, HTTPClient::ResponseCode p_response_code, PackedStringArray p_headers, PackedByteArray p_body) {
	if (p_response_code != 200) {
		return;
	}

	Dictionary response = JSON::parse_string(p_body.get_string_from_utf8());
	if (!response.has("tag_name")) {
		return;
	}

	String current_version = String(GIT_COMMIT_TAG);
	String latest_version = response["tag_name"];

	print_verbose("NDI: Current version: ", current_version, " / Latest version: ", latest_version);

	if (current_version.is_empty()) {
		return;
	}

	if (latest_version != current_version) {
		WARN_PRINT_ED("New version of Godot NDI available (" + latest_version + ")");
	}
}
