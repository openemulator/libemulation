# libemulation building instructions

These are the building instructions for libemulation, a cross-platform emulation framework of many legacy computer systems.

## macOS
We aim to be portable, but currently the only officially supported and tested platform is macOS.

To compile libemulation for macOS you need Xcode and several external libraries not available by default. The simplest way to install them is using Homebrew.

### Setup
Install Homebrew from the official site: <http://brew.sh/>

The official tool for building libemulation is CMake.

	brew update
	brew install cmake

The required version of CMake is at least 3.11.

### Dependencies
Use Homebrew to install the required dependencies:

	brew install libpng libxml2 libzip portaudio libsndfile libsamplerate

### Build
With the dependencies installed, we're now ready to build libemulation. Run from the cloned repository:

	cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release
	cmake --build build --config Release

This will create a static library file `libemulation.a` in the build directory.
