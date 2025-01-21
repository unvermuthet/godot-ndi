# godot-ndi

> GDExtension for NDI


Extends the [VideoStream](https://docs.godotengine.org/en/stable/classes/class_videostream.html) and [VideoStreamPlayer](https://docs.godotengine.org/en/stable/classes/class_videostreamplayer.html) classes to implement NDI playback.

The extension can currently only receive video, because that's all I need.
It still serves as a great starting point anyone looking to build a feature complete implementation. Audio code is commented out since its delayed to the point of being unusable.

## Download

Check under `Releases` or get Build Artifacts under `Actions`

## Building from source

Dev Container configuration files are included. Run `scons` to compile for Linux. If you want to compile for Windows use MSYS2.