# Homebrodot

A port of the Godot open source game engine to various homebrew platforms,
via libraries provided by [devkitPro](https://devkitpro.org) and [VitaSDK] respectively.

See the [original README](https://github.com/godotengine/godot/blob/3.x/README.md)
for info about Godot.


## Releases

See [the GitHub releases page](https://github.com/Hombrodot/godot/releases)
for pre-built editors for Windows, macOS, and Linux,
and pre-built engines for all supported platforms.

## How do I export my game?

Instructions for platforms supported by Homebrodot can be found below.

- [Nintendo Switch](./BUILDING-Switch.md#Exporting-for-Nintendo-Switch)
- [PS Vita / PS TV](./BUILDING-Vita.md#Exporting-for-PS-Vita-or-PS-TV)

Otherwise, please see the [Godot docs].

## How to build

**You shouldn't need to build the engine if you use a release** as the templates should be included -
**these instructions are for engine developers!**

Please see the [Godot docs]
for compilation instructions for officially Godot platform.
***These are also useful for learning the basic requirements.***

Instructions for platforms supported by Homebrodot can be found here:

- [Nintendo Switch](./BUILDING-Switch.md#Building-for-Nintendo-Switch)
- [PS Vita / PS TV](./BUILDING-Vita.md#Building-for-PS-Vita-or-PS-TV)

## How can I get help?

Either make an issue on this repo, or join the [Discord](https://discordapp.com/invite/yUC3rUk)!

## Acknowledgements

* Thanks to [@devkitPro] and [@switchbrew] for producing devkitA64/libnx.
* Thanks to the [@VitaSDK] team for producing VitaSDK.
* Extra special thanks to [@fincs] from devkitPro.<br>
The Switch port would have never been possible without their port of the open source nouveau graphics driver.
* Thanks to [all the developers](./AUTHORS.md#developers) who've contributed to Godot!

[Godot docs]:https://docs.godotengine.org/en/3.5/development/compiling/index.html

[@devkitPro]:https://github.com/devkitPro/
[@switchbrew]:https://github.com/switchbrew/
[@VitaSDK]:https://github.com/VitaSDK/
[@fincs]:https://github.com/fincs/
