# godot-wii-u
WIP port of Godot 4 to Wii U

Uses Wii U Toolchain.

This repo will only contain the relevant folders to add to an existing copy of the Godot source code.


# Progress
## Vital core things
- [ ] Threading (coreinit)
  - [ ] Thread (coreinit/thread)
  - [ ] Mutex (coreinit/mutex)
  - [ ] Semaphore (coreinit/semaphore)
- [ ] Crash handling
- [ ] OS integration
  - [ ] Home Menu (ProcUI)
  - [ ] Keyboard overlay (swkbd)
  - [x] Software information
  - [x] Hardware information
  - [ ] Time (Time)

## Secondary vital core things
- [ ] Filesystem access
- [ ] Graphics (gx2, gx2r)
- [ ] Audio (sndcore2)
- [ ]  Input
  - [ ] Gamepad (VPAD)
  - [ ] Wii remotes (KPAD)

## Nice QoL things
- [ ] Networking
  - [ ] HTTPClient

# Compiling

I'm adding this here just so people don't ask about it. The SCons script to build the platform isn't done, nor will I worry about it until I have enough done to make it work trying to compile to see if it works.

If you have any questions, join [The Homebrodot Discord](https://discord.gg/mYzXDke5yv),[my Discord](https://discord.com/invite/Mu6YUEmerN) or make an issue here on GitHub.
