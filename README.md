# Vokiso

*The next generation of voice conferencing*

Current voice/video conferencing applications have one major flaw: everyone
is shouting into everyone else's ears. More specifically, it's hard to interject
in a conversation because really only one person can talk at a time. Vokiso
aims to solve that problem by projecting voices into a 3D room so that the
concept of direction and distance can be heard. This small addition will
fundamentally change voice communication and put digital communication
on the same level as face to face conversation.

To eliminate potentially expensive server costs, Vokiso runs entirely peer to
peer.

## Download

Hold up, this software hasn't even been fully built yet. The MVP is currently
in the works, so come back later when we actually have something built and
have implemented continuous deployment.

## Development

**Current State:**

The software is written entirely in C++. Here's an overview of the basic code flow:

 - First, someone hosts a call. This sets up port forwarding with [miniupnpc](http://miniupnp.free.fr/)
   from a random public port to a random local port that the application listens on. 
 - Then, the host's public ip and port get encoded into a 5 word string that the host
   shares with meeting attendees.
 - Meeting attendees enter this code and join. The clients communicate to the host via
   TCP sockets using the [kissnet](https://github.com/Ybalrid/kissnet) wrapper
 - The clients record and generate audio using [OpenAL](https://openal.org/).
 - Each client communicates through a custom protocol consisting of either audio messages
   or positional messages, each prefixed by a randomly generated user id.
 - The server simply forwards messages between clients

At the moment, the code above was hackily written but doesn't work yet due to tons of bugs.
The plan is to refactor and write unit tests to get the written functionality working.

**Roadmap:**

 - Add some super simple voice activity detection (even like RMS) to only send chunks when talking
 - Control position with keyboard
 - Normalize audio volume
 - Use [SDL](https://www.libsdl.org/) or something like [bgfx](https://github.com/bkaradzic/bgfx)
   to make simple 2D/3D interface to view participants and move your location.
 - If network performance is a problem, reduce bandwidth through one/some of the following:
    - Encode audio (ie. mp3)
    - Multiple broadcaster nodes (ie. cross between central host and mesh network)
 - Once actually usable, implement continuous deployment with something like GitHub Actions to build
   Windows, Mac, and Linux executables on each commit
 - Built a pretty static website and deploy on GitHub Pages

**Compiling:**

Note: audio doesn't even work yet and the code is a mess, so you probably want to wait a few days until I get it to a working
state.

Here's the process for compilation:

 - Install the build tool, [meson](https://mesonbuild.com/Quick-guide.html).
 - Try building with the following:


```bash
git clone https://github.com/MatthewScholefield/vokiso
cd vokiso
meson build  # This might fail, see below
cd build
ninja  # This is similar to running "make"
```

If `meson build` fails to find dependencies, you should make sure you have installed each of the following:

 - [OpenAL](https://openal.org/)
 - [miniupnpc](http://miniupnp.free.fr/)
 - `pthread`

If it still doesn't compile, reach out to me because it's probably an issue with the project's portability.
