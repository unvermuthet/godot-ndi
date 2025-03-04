# godot-ndi [![Made with Godot](https://img.shields.io/badge/Made%20with-Godot-478CBF?style=flat&logo=godot%20engine&logoColor=white)](https://godotengine.org) [![Build GDExtension](https://github.com/unvermuthet/godot-ndi/actions/workflows/builds.yml/badge.svg)](https://github.com/unvermuthet/godot-ndi/actions/workflows/builds.yml)

> Integrates the NDI® SDK with Godot
> 
> [ NDI® is a registered trademark of Vizrt NDI AB ]

Extends the [VideoStream](https://docs.godotengine.org/en/stable/classes/class_videostream.html) and VideoStreamPlayback classes to implement **NDI playback**.
For **outputting the Viewport** as an NDI source, add the NDIOutput Node to the scene.
Check out the **demo scenes** included with the releases.

This was born out of work on my DMX Lighting Control Software. I'm really into Live Event Productions, AV, Projection Mapping, TechArt and Broadcasting. [So reach out](https://discord.com/users/203583245223198722) or [show off](https://github.com/unvermuthet/godot-ndi/discussions/categories/show-and-tell) what you made with Godot NDI!

<!-- [![Static Badge](https://img.shields.io/badge/unvermuthet-gray?style=flat&logo=discord&logoColor=white&labelColor=%235865F2)](https://discord.com/users/203583245223198722) -->

## Install

> [!IMPORTANT]
> - Only compatible with `Godot 4.4-stable` and later
> - The NDI Runtime is required ([Windows](http://ndi.link/NDIRedistV6), [MacOS](http://ndi.link/NDIRedistV6Apple), [Linux (Flatpak incomaptible)](https://github.com/DistroAV/DistroAV/wiki/1.-Installation#linux))
> - Your application needs to comply with the [NDI SDK license](http://ndi.link/ndisdk_license).

Download the latest version under `Releases` and install by extracting the ZIP file in your Godot project.
Alternatively you can install the extension from Godot's Asset Library.

## Support

In the production industries, software usually comes with exorbitant licensing fees. If this project has been useful to you and you want to show appreciation for my time and effort, consider a donation.

For **comercial** support and exclusive alternative licensing, contact me at [godot-ndi@muth.pro](mailto:godot-ndi@muth.pro)

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/I2I51A7ZC3)

## Building from source

To help you get going, I've configured a [Development Container](https://containers.dev/) with everything setup to target Windows or Linux. Just run `scons`!

If you want to configure the environment yourself, follow [Godot's "Building from source" Guide](https://docs.godotengine.org/en/latest/contributing/development/compiling/). This project uses an identical build system.

## License

(C) 2025 Henry Muth - unvermuthet

godot-ndi is subject to the terms of the [Mozilla Public License, v. 2.0](LICENSE).

Generally, changes to the source code must be made available under the same license. See license text.

### Attribution suggestion
```
godot-ndi
    Licensed under MPL-2.0
    (C) 2025 Henry Muth - unvermuthet
    https://github.com/unvermuthet/godot-ndi
```
