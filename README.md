# godot-ndi

> GDExtension for NDI

[![Made with Godot](https://img.shields.io/badge/Made%20with-Godot-478CBF?style=flat&logo=godot%20engine&logoColor=white)](https://godotengine.org)


Extends the [VideoStream](https://docs.godotengine.org/en/stable/classes/class_videostream.html) and [VideoStreamPlayer](https://docs.godotengine.org/en/stable/classes/class_videostreamplayer.html) classes to implement NDI playback.

The extension can **currently only receive video**, because that's all I need.
It still serves as a great starting point anyone looking to build a feature complete implementation.

This is early stages. I'm happy to hear suggestions and incorperate improvements. [![Static Badge](https://img.shields.io/badge/unvermuthet-gray?style=flat&logo=discord&logoColor=white&labelColor=%235865F2)](https://discord.com/users/203583245223198722)

## Example

```GDScript
@onready var player: VideoStreamPlayer = %VideoStreamPlayer
var ndi_find: NDIFind
var stream: VideoStreamNDI

func _ready() -> void:
    ndi_find = NDIFind.new()

    while true:
        stream = ndi_find.get_sources().pop_front()
        if stream != null:
            break
            
    player.stream = stream
    player.play()
```

## Download

Check under `Releases` or get Build Artifacts under `Actions`. Install by extracting the zip anywhere is your Godot project. The NDI Runtime is also required.

[NDI Runtime for Windows](http://ndi.link/NDIRedistV6)

[NDI Runtime for MacOS](http://ndi.link/NDIRedistV6Apple)

[NDI Runtime for Linux](https://github.com/DistroAV/DistroAV/wiki/1.-Installation#linux) (Doesn't work with flatpak editor since that's sandboxed)

## Building from source

There is a Dev Container configured that will allow you to target Linux.
If you want to target Windows install MSYS2 and run `scons use_mingw=yes`.

## License

(C) 2025 Henry Muth - unvermuthet

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.

