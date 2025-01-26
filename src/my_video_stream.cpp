#include "my_video_stream.hpp"

void MyVideoStream::_bind_methods() {
}

MyVideoStream::MyVideoStream() {
}

MyVideoStream::~MyVideoStream() {
}

Ref<VideoStreamPlayback> MyVideoStream::_instantiate_playback() {
	return memnew(MyVideoStreamPlayback);
}