/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <godot_cpp/classes/http_request.hpp>

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH ""
#endif

#ifndef GIT_COMMIT_TAG
#define GIT_COMMIT_TAG ""
#endif

using namespace godot;

class NDIVersionCheck : public HTTPRequest {
	GDCLASS(NDIVersionCheck, HTTPRequest)

public:
	NDIVersionCheck();
	~NDIVersionCheck();
	static Dictionary get_copyright_info();

protected:
	static void _bind_methods();
	void _notification(int p_what);
	void _on_request_completed(HTTPRequest::Result p_result, HTTPClient::ResponseCode p_response_code, PackedStringArray p_headers, PackedByteArray p_body);
};
