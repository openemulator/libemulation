cmake_minimum_required(VERSION 3.11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set up as a C++ project
project(emulation CXX)

# Set up convenient directory variables
set(source_directory ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Dependencies
find_package(PNG REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(LibZip REQUIRED)
find_package(PortAudio REQUIRED)
find_package(LibSndFile REQUIRED)
find_package(LibSampleRate REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

# Compile-time definitions
add_definitions(-DGL_GLEXT_PROTOTYPES)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-D_LINUX)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux") 

# Get sources and headers for the OE libraries
include(${source_directory}/libdiskimage.cmake)
include(${source_directory}/libemulation.cmake)
include(${source_directory}/libemulation-hal.cmake)
include(${source_directory}/libutil.cmake)

# Setup include directories
include_directories(
  ${libdiskimage_include}
  ${emulation_hal_include}
  ${emulation_include}
  ${libutil_include}
  ${PNG_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${LIBZIP_INCLUDE_DIR}
  ${PORTAUDIO_INCLUDE_DIRS}
  ${LIBSNDFILE_INCLUDE_DIR}
  ${LIBSAMPLERATE_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
)

# libemulation as a static library
add_library(emulation STATIC
  ${libdiskimage}
  ${emulation_hal}
  ${emulation}
  ${libutil}
)

target_link_libraries(emulation
  ${PNG_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${LIBZIP_LIBRARY}
  ${PORTAUDIO_LIBRARIES}
  ${LIBSNDFILE_LIBRARIES}
  ${LIBSAMPLERATE_LIBRARIES}
  ${GLUT_LIBRARIES}
  ${OPENGL_LIBRARIES}
)
