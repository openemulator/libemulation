# libdiskimage - disk image emulation
set(_libdiskimage_dir ${source_directory}/libdiskimage)

# Sources
set(libdiskimage
  ${_libdiskimage_dir}/DI2IMGBackingStore.cpp
  # TODO ${_libdiskimage_dir}/DIApple35DiskStorage.cpp
  ${_libdiskimage_dir}/DIApple525DiskStorage.cpp
  ${_libdiskimage_dir}/DIATABlockStorage.cpp
  ${_libdiskimage_dir}/DIBackingStore.cpp
  ${_libdiskimage_dir}/DIBlockStorage.cpp
  ${_libdiskimage_dir}/DICommon.cpp
  ${_libdiskimage_dir}/DIDC42BackingStore.cpp
  ${_libdiskimage_dir}/DIDDLDiskStorage.cpp
  ${_libdiskimage_dir}/DIDiskStorage.cpp
  ${_libdiskimage_dir}/DIFDIDiskStorage.cpp
  ${_libdiskimage_dir}/DIFileBackingStore.cpp
  ${_libdiskimage_dir}/DILogicalDiskStorage.cpp
  ${_libdiskimage_dir}/DIRAMBackingStore.cpp
  ${_libdiskimage_dir}/DIRAWBlockStorage.cpp
  ${_libdiskimage_dir}/DIV2DDiskStorage.cpp
  ${_libdiskimage_dir}/DIVDIBlockStorage.cpp
  ${_libdiskimage_dir}/DIVMDKBlockStorage.cpp
  ${_libdiskimage_dir}/DIWozDiskStorage.cpp
)

# Headers
set(libdiskimage_include ${_libdiskimage_dir})
