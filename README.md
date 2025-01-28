# godot-ndi

> GDExtension for NDI

Licensed under MPL 2.0

[![Made with Godot](https://img.shields.io/badge/Made%20with-Godot-478CBF?style=flat&logo=godot%20engine&logoColor=white)](https://godotengine.org)

The extension can **currently only receive video and audio**, because that's all I need. Viewport output as NDI is planned. Extends the [VideoStream](https://docs.godotengine.org/en/stable/classes/class_videostream.html) and [VideoStreamPlayer](https://docs.godotengine.org/en/stable/classes/class_videostreamplayer.html) classes to implement NDI playback.


This is early stages. I'm happy to hear suggestions and incorperate improvements. [![Static Badge](https://img.shields.io/badge/unvermuthet-gray?style=flat&logo=discord&logoColor=white&labelColor=%235865F2)](https://discord.com/users/203583245223198722)

## Example

```GDScript
extends Node

@onready var finder: NDIFinder = %NDIFinder
@onready var player: VideoStreamPlayer = %VideoStreamPlayer

func _ready() -> void:
    finder.connect("sources_changed", self.sources_changed)
    
func sources_changed():
    var sources := finder.get_sources()
    
    if not sources.is_empty():
        var source := sources.front() as VideoStreamNDI
        print(source.name)
        player.stream = source
        player.play()

```

## Download

Check under `Releases` or get Build Artifacts under `Actions`. Install by extracting the zip anywhere is your Godot project. The NDI Runtime is also required.

- [NDI Runtime for Windows](http://ndi.link/NDIRedistV6)
- [NDI Runtime for MacOS](http://ndi.link/NDIRedistV6Apple)
- [NDI Runtime for Linux](https://github.com/DistroAV/DistroAV/wiki/1.-Installation#linux) (Doesn't work with flatpak editor since that's sandboxed)

## Building from source

There is a Dev Container configured that will allow you to target Linux.
If you want to target Windows install MSYS2 and run `scons use_mingw=yes`.

## License

Generally, improvements to the source code must be made available under the same license. See license text.

(C) 2025 Henry Muth - unvermuthet

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.
