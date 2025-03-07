/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "includes.hpp"

NDIVersionCheck::NDIVersionCheck() {
}

NDIVersionCheck::~NDIVersionCheck() {
}

void NDIVersionCheck::_bind_methods() {
}

void NDIVersionCheck::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			HTTPRequest *request = memnew(HTTPRequest);
			add_child(request);

			request->connect("request_completed", callable_mp(this, &NDIVersionCheck::_on_request_completed));
			request->request("https://api.github.com/repos/unvermuthet/godot-ndi/releases/latest");
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
		get_editor_interface()->get_editor_toaster()->push_toast("New version of Godot NDI available (" + latest_version + ")", EditorToaster::SEVERITY_INFO);
	}
}
