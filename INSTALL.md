# libemulation building instructions

These are the building instructions for libemulation, a cross-platform emulator of many legacy computer systems.

## Mac OS X
We aim to be portable, but currently the only offically supported and tested platform is macOS.

To compile OpenEmulator for Mac OS X you need Xcode and several external libraries not available by default. The simplest way to install them is Homebrew.

### Setup
Install Homebrew from the official site:

	http://brew.sh/

The official tool for building libemulation is CMake.

	brew update
	brew install cmake

The required version of CMake is at least 3.11.

### Install dependencies
Use Homebrew to install the required dependencies:

	brew update
	brew install libpng libxml2 libzip portaudio libsndfile libsamplerate

### Build
With the dependencies installed, we're now ready to build libemulation. Run from the cloned repository:

	cmake -H. -B_builds
	cmake --build _builds

This will create a static library file `libemulation.a` in the build directory.
