# godot-ndi [![Made with Godot](https://img.shields.io/badge/Made%20with-Godot-478CBF?style=flat&logo=godot%20engine&logoColor=white)](https://godotengine.org) [![Build GDExtension](https://github.com/unvermuthet/godot-ndi/actions/workflows/builds.yml/badge.svg)](https://github.com/unvermuthet/godot-ndi/actions/workflows/builds.yml)

> Integrates the NDI® SDK with Godot

> [ NDI® is a registered trademark of Vizrt NDI AB ]

Extends the [VideoStream](https://docs.godotengine.org/en/stable/classes/class_videostream.html) and VideoStreamPlayback classes to implement **NDI playback**.
For **outputting the Viewport** as an NDI source, add the NDIOutput Node to the scene.
Check out the **demo scenes** included with the releases.

I'm happy to hear suggestions and incorperate improvements. You can find my contact info on my Github profile. 

<!-- [![Static Badge](https://img.shields.io/badge/unvermuthet-gray?style=flat&logo=discord&logoColor=white&labelColor=%235865F2)](https://discord.com/users/203583245223198722) -->

## Install

> [!IMPORTANT]
> - Minimum Godot version is `4.4-beta3`

Download the latest version under `Releases`. Install by extracting the ZIP file anywhere in your Godot project.

The NDI Runtime is also required.
- [NDI Runtime for Windows](http://ndi.link/NDIRedistV6)
- [NDI Runtime for MacOS](http://ndi.link/NDIRedistV6Apple)
- [NDI Runtime for Linux](https://github.com/DistroAV/DistroAV/wiki/1.-Installation#linux) (Doesn't work with flatpak editor since that's sandboxed)

Your application needs to comply with the [NDI SDK license](http://ndi.link/ndisdk_license).

## Building from source

This project is based on the typical [Godot build system](https://docs.godotengine.org/en/latest/contributing/development/compiling/).

There is a Dev Container configured that will allow you to target Linux.
If you want to target Windows you could install MSYS2 and run `scons use_mingw=yes` for example.

## License

Generally, improvements to the source code must be made available under the same license. See license text.

(C) 2025 Henry Muth - unvermuthet

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at https://mozilla.org/MPL/2.0/.

#

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/I2I51A7ZC3)
