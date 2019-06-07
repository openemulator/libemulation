# libemulation
[![Build Status](https://travis-ci.org/openemulator/libemulation.svg?branch=master)](https://travis-ci.org/openemulator/libemulation)

This is libemulation, a portable emulation framework powering emulators such as [OpenEmulator](https://github.com/openemulator/openemulator).

## About
libemulation provides a component framework and library to accurately emulate legacy computer systems.

Emulation is done by connecting emulated components together using an XML description called "Emulation Description Language". It is thus very easy to interconnect different CPUs, address decoders, memories, chip-sets or input/output devices to fully emulate different computer systems.

It is also easy to add your own software components to the framework. You are encouraged to submit your own emulations as pull requests, so your work gets all the attention it deserves.

## Usage
libemulation is implemented in C++ and can be linked with your emulator application as a static library. We aim to be portable, but currently the only offically supported and tested platform is macOS. Refer to [INSTALL.md](INSTALL.md) for instructions how to build libemulation.

## Feedback
If you find a bug, or would like a specific feature, please report it at:

[https://github.com/openemulator/libemulation/issues](https://github.com/openemulator/libemulation/issues)

If you like to contribute to the project, fork the repository on Github and send us your pull requests!

## License
libemulation is released under the [GNU General Public License v3](COPYING).
