# Building for PS Vita or PS TV

These steps must be done under Linux / Windows Subsystem for Linux (WSL).

## Dependencies

If you haven't done so already, install the dependencies called for by the [official Godot docs](https://docs.godotengine.org/en/3.5/development/compiling/compiling_for_x11.html)

1. Install [VitaSDK]
   <details>

   ```sh
   # Clone and bootstrap
   git clone https://github.com/vitasdk/vdpm
   cd vdpm
   ./bootstrap-vitasdk.sh
   # Add to PATH (add this to ~/.bashrc for persistence after reboot)
   export VITASDK=/usr/local/vitasdk
   export PATH=$VITASDK/bin:$PATH # add vitasdk tool to $PATH
   # Finish install
   ./install-all.sh
   # Update
   vitasdk-update
   ```

   </details>
2. Install [PVR-PSP2]
   <details>

   ```sh
   # Clone, build and install
   git clone https://github.com/isage/vita-packages-extra
   cd vita-packages-extra/pvr_psp2
   vita-makepkg
   vdpm *-arm.tar.xz
   ```

   </details>

## Building

1. Run `scons platform=vita` in the root of the repo.
   Add `target=release` to the end to build for release instead of debug.

# Exporting for PS Vita or PS TV

The [editor releases] add a `Vita` exporter to the list -
it will generate a homebrew VPK file  that can be installed on
your PlayStation Vita or PlayStation TV with VitaShell.

# Need help?

Either make an issue on this repo, or join the [Discord](https://discordapp.com/invite/yUC3rUk)!

[editor releases]:https://github.com/Homebrodot/godot/releases/latest
[VitaSDK]:https://vitasdk.org/
[PVR-PSP2]:https://github.com/GrapheneCt/PVR_PSP2
