# Building for Nintendo Switch

## Dependencies

1. Install [devkitPro pacman]:
2. Install the required packages from the terminal
   ```sh
   # On systems without pacman pre-installed (Such as Debian, Ubuntu):
   sudo dkp-pacman -S switch-pkg-config switch-freetype switch-bulletphysics switch-libtheora switch-libpcre2 switch-mesa switch-opusfile switch-mbedtls switch-libwebp switch-libvpx switch-miniupnpc switch-libzstd switch-wslay
   ```
   OR
   ```sh
   # On systems with pacman pre-installed (Such as Arch, Manjaro, MSYS2):
   sudo pacman -S switch-pkg-config switch-freetype switch-bulletphysics switch-libtheora switch-libpcre2 switch-mesa switch-opusfile switch-mbedtls switch-libwebp switch-libvpx switch-miniupnpc switch-libzstd switch-wslay

## Building

1. Run `scons platform=switch` in the root of the repo.<br>
   Add `target=release` to the end to build for release instead of debug.
2. Run `misc/scripts/build/create-switch-release.sh` in the root of the repo
to create an installable release for your Switch.
3. Run `misc/scripts/build/create-switch-template.sh` in the root of the repo
to create an export template for your editor.

# Exporting for Nintendo Switch

The [editor releases] add a `Switch` exporter to the list -
it can generate a Switch homebrew NRO or a generic Godot PCK file
that can be transferred to your Switch.

If additional debugging is required, sending the NRO file via nxlink
(make sure to send it to the right place with the -p argument)
will allow the output of the console to be viewed on PC.

# Need help?

Either make an issue on this repo, or join the [Discord](https://discordapp.com/invite/yUC3rUk)!

[editor releases]:https://github.com/Homebrodot/godot/releases/latest
[devkitPro pacman]:https://devkitpro.org/wiki/devkitPro_pacman
