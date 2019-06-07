# libemulation-hal - hardware abstraction layer

set(_libemulation_hal_dir ${source_directory}/libemulation-hal)

# Sources
set(emulation_hal
  ${_libemulation_hal_dir}/HIDJoystick.cpp
  ${_libemulation_hal_dir}/OEMatrix3.cpp
  ${_libemulation_hal_dir}/OEVector.cpp
  ${_libemulation_hal_dir}/OpenGLCanvas.cpp
  ${_libemulation_hal_dir}/PAAudio.cpp
)

set(emulation_hal_include ${_libemulation_hal_dir})