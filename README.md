# godot-ndi [![Made with Godot](https://img.shields.io/badge/Made%20with-Godot-478CBF?style=flat&logo=godot%20engine&logoColor=white)](https://godotengine.org) [![Build GDExtension](https://github.com/unvermuthet/godot-ndi/actions/workflows/builds.yml/badge.svg)](https://github.com/unvermuthet/godot-ndi/actions/workflows/builds.yml) [![Read the docs](https://img.shields.io/readthedocs/godot-ndi)](https://godot-ndi.readthedocs.io)

> Integrates the NDI® SDK with Godot
>
> [ NDI® is a registered trademark of Vizrt NDI AB ]

Adds a new [VideoStream](https://godot-ndi.readthedocs.io/stable/classes/class_videostreamndi.html) class to implement **NDI playback**.
For **outputting the Viewport** as an NDI source, add the [NDIOutput](https://godot-ndi.readthedocs.io/stable/classes/class_ndioutput.html) Node to the scene.
Check out the **demo scenes** included with all releases.

This was born out of work on my DMX Lighting Control Software. I'm fascinated by Live Event Productions, Projection Mapping, TechArt, and Broadcasting. [So reach out](https://discord.com/users/203583245223198722) or [show off](https://github.com/unvermuthet/godot-ndi/discussions/categories/show-and-tell) what you made with Godot NDI!

<!-- [![Static Badge](https://img.shields.io/badge/unvermuthet-gray?style=flat&logo=discord&logoColor=white&labelColor=%235865F2)](https://discord.com/users/203583245223198722) -->

![Demo GIF](https://github.com/user-attachments/assets/159eb455-b0de-41bb-972f-be0817771455)

> This GIF only shows local transmission, but imagine this over the network!

## Install

> [!IMPORTANT]
> - Only compatible with `Godot 4.4-stable` and later
> - Your application needs to comply with the [NDI SDK license](http://ndi.link/ndisdk_license).
> - The NDI Runtime is required ([Windows](http://ndi.link/NDIRedistV6), [MacOS](http://ndi.link/NDIRedistV6Apple), [Linux (Flatpak incompatible)](https://github.com/DistroAV/DistroAV/wiki/1.-Installation#linux))

Download the latest version under `Releases` and install by extracting the ZIP file in your Godot project.
Alternatively, you can install the extension from Godot's Asset Library.

## Documentation

Read the documentation at [godot-ndi.readthedocs.io](https://godot-ndi.readthedocs.io)

## Support

For ***commercial*** support, exclusive alternative licensing, or [NDI Advanced SDK](https://ndi.video/for-developers/ndi-advanced/) integration, contact me at [godot-ndi@muth.pro](mailto:godot-ndi@muth.pro).

In the production industries, software usually comes with exorbitant licensing fees. If this project has been useful to you and you want to show appreciation for my time and effort, consider a donation.

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/I2I51A7ZC3)

## Building from source

To help you get going, I've configured a [Development Container](https://containers.dev/) with everything set up to target Windows or Linux. Just run `scons` or check `scons --help`!

If you want to configure the environment yourself, follow [Godot's "Building from source" Guide](https://docs.godotengine.org/en/latest/contributing/development/compiling/). This project uses an identical build system.

## License

**(C) 2025-present Henry Muth - unvermuthet and Godot NDI contributors**

### Software

Subject to the terms of the [Mozilla Public License, v. 2.0](https://www.mozilla.org/en-US/MPL/2.0/)

### Documentation

Licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/)

### Attribution suggestion

```
godot-ndi
    Licensed under MPL-2.0
    (C) 2025-present Henry Muth - unvermuthet and Godot NDI contributors
    https://github.com/unvermuthet/godot-ndi
```
