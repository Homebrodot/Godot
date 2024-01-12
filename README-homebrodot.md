# Homebrodot

It's a port of the Godot open source game engine to homebrew platforms,
using homebrew libraries such as LibNX and [VitaSDK].
See the original [README.md](./README.md) for info about Godot.

## Releases

See [the GitHub releases page](https://github.com/Hombrodot/godot/releases)
for pre-built editors for Windows, macOS, and Linux,
and export templates for all supported platforms.

## How do I export my game?

Instructions for platforms supported by Homebrodot can be found below.

- [Nintendo Switch](./BUILDING-switch.md#Exporting-for-Nintendo-Switch)
- [PlayStation Vita / PS TV](./BUILDING-vita.md#Exporting-for-PS-Vita-or-PS-TV)

Otherwise, please see the [Godot docs].

## How to build

**You shouldn't need to build the engine if you use a release** as the templates should be included -
**these instructions are for engine developers!**

Please see the [Godot docs]
for compilation instructions for officially Godot platform.
***These are also useful for learning the basic requirements.***

Instructions for platforms supported by Homebrodot can be found here:

- [Nintendo Switch](./BUILDING-switch.md#Building-for-Nintendo-Switch)
- [PlayStation Vita / PS TV](./BUILDING-vita.md#Building-for-PS-Vita-or-PS-TV)

## How can I get help?

Either make an issue on this repo, or join the [Discord](https://discord.gg/yUC3rUk "Homebrodot - Godot Homebrew Hub")!

# Credits

- Thanks to [@devkitPro] and [@switchbrew] for producing devkitA64/libnx.
- Thanks to the [@VitaSDK] team for producing [VitaSDK].
- Extra special thanks to [@fincs] from devkitPro.<br>
  The Switch port would have never been possible without their port of the open source nouveau graphics driver.
- Thanks to [@cpasjuste] for help with development.
- Thanks to [@fhidalgosola]/utnad for their Switch port / help with development.

[@devkitPro]:https://github.com/devkitPro
[@switchbrew]:https://github.com/switchbrew
[@fincs]:https://github.com/fincs
[@cpasjuste]:https://github.com/Cpasjuste
[@fhidalgosola]:https://github.com/fhidalgosola

[@VitaSDK]:https://github.com/VitaSDK
[VitaSDK]:https://vitasdk.org
