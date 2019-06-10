# libemulation - emulation libraries

set(_libemulation_dir ${source_directory}/libemulation)

# Sources
set(emulation
  # Core libraries
  ${_libemulation_dir}/Core/OECommon.cpp
  ${_libemulation_dir}/Core/OEComponent.cpp
  ${_libemulation_dir}/Core/OEComponentFactory.cpp
  ${_libemulation_dir}/Core/OEDevice.cpp
  ${_libemulation_dir}/Core/OEDocument.cpp
  ${_libemulation_dir}/Core/OEEmulation.cpp
  ${_libemulation_dir}/Core/OEImage.cpp
  ${_libemulation_dir}/Core/OEPackage.cpp
  ${_libemulation_dir}/Core/OESound.cpp
  # Generic libaries
  ${_libemulation_dir}/Implementation/Generic/AddressDecoder.cpp
  ${_libemulation_dir}/Implementation/Generic/AddressMapper.cpp
  ${_libemulation_dir}/Implementation/Generic/AddressMasker.cpp
  ${_libemulation_dir}/Implementation/Generic/AddressMux.cpp
  ${_libemulation_dir}/Implementation/Generic/AddressOffset.cpp
  ${_libemulation_dir}/Implementation/Generic/ATAController.cpp
  ${_libemulation_dir}/Implementation/Generic/ATADevice.cpp
  ${_libemulation_dir}/Implementation/Generic/Audio1Bit.cpp
  ${_libemulation_dir}/Implementation/Generic/AudioCodec.cpp
  ${_libemulation_dir}/Implementation/Generic/AudioPlayer.cpp
  ${_libemulation_dir}/Implementation/Generic/ControlBus.cpp
  ${_libemulation_dir}/Implementation/Generic/FloatingBus.cpp
  ${_libemulation_dir}/Implementation/Generic/JoystickMapper.cpp
  ${_libemulation_dir}/Implementation/Generic/Monitor.cpp
  ${_libemulation_dir}/Implementation/Generic/Proxy.cpp
  ${_libemulation_dir}/Implementation/Generic/RAM.cpp
  ${_libemulation_dir}/Implementation/Generic/ROM.cpp
  ${_libemulation_dir}/Implementation/Generic/VRAM.cpp
  # Applied Engineering
  ${_libemulation_dir}/Implementation/AE/AERamFactor.cpp
  # Apple
  ${_libemulation_dir}/Implementation/Apple/Apple1ACI.cpp
  ${_libemulation_dir}/Implementation/Apple/Apple1IO.cpp
  ${_libemulation_dir}/Implementation/Apple/Apple1Terminal.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleDiskDrive525.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleDiskIIInterfaceCard.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleGraphicsTablet.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleGraphicsTabletInterfaceCard.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIAddressDecoder.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIAudioIn.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIAudioOut.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIDisableC800.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIEAddressDecoder.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIEKeyboard.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIEMMU.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIEVideo.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIFloatingBus.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIGamePort.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIIAddressDecoder.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIIBeeper.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIIDiskIO.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIIGamePort.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIIKeyboard.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIIMOS6502.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIIRTC.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIISystemControl.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIIVideo.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIKeyboard.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIISlotController.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIISystemControl.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleIIVideo.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleLanguageCard.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleSilentype.cpp
  ${_libemulation_dir}/Implementation/Apple/AppleSilentypeInterfacecard.cpp
  # Don't Ask
  ${_libemulation_dir}/Implementation/Don\'t\ Ask/SAMDACCard.cpp
  # MOS
  ${_libemulation_dir}/Implementation/MOS/MOS6502.cpp
  # TODO ${_libemulation_dir}/Implementation/MOS/MOS6509.cpp
  ${_libemulation_dir}/Implementation/MOS/MOS6522.cpp
  ${_libemulation_dir}/Implementation/MOS/MOS6530.cpp
  ${_libemulation_dir}/Implementation/MOS/MOS6551.cpp
  ${_libemulation_dir}/Implementation/MOS/MOSKIM1IO.cpp
  ${_libemulation_dir}/Implementation/MOS/MOSKIM1PLL.cpp
  # Motorola
  ${_libemulation_dir}/Implementation/Motorola/MC6821.cpp
  ${_libemulation_dir}/Implementation/Motorola/MC6845.cpp
  # National Semiconductor
  ${_libemulation_dir}/Implementation/National/MM58167.cpp
  # R&D Automation
  ${_libemulation_dir}/Implementation/RD/RDCFFA.cpp
  # Ricoh
  # TODO ${_libemulation_dir}/Implementation/Ricoh/RP2A03.cpp
  # Videx
  ${_libemulation_dir}/Implementation/Videx/VidexVideoterm.cpp
  # Western Design Center
  ${_libemulation_dir}/Implementation/WDC/W65C02S.cpp
  # TODO ${_libemulation_dir}/Implementation/WDC/W65C816S.cpp
  # Zilog
  # TODO ${_libemulation_dir}/Implementation/Zilog/Z80.cpp
  # Interface
  ${_libemulation_dir}/Interface/Generic/MemoryInterface.cpp
  ${_libemulation_dir}/Interface/Host/AudioInterface.cpp
)

# Headers

set(emulation_include
  ${_libemulation_dir}/Implementation/Generic
  ${_libemulation_dir}/Core
  ${_libemulation_dir}/Implementation/AE
  ${_libemulation_dir}/Implementation/Apple
  ${_libemulation_dir}/Implementation/Don\'t\ Ask
  ${_libemulation_dir}/Implementation/MOS
  ${_libemulation_dir}/Implementation/Motorola
  ${_libemulation_dir}/Implementation/National
  ${_libemulation_dir}/Implementation/RD
  ${_libemulation_dir}/Implementation/Videx
  ${_libemulation_dir}/Implementation/WDC
  ${_libemulation_dir}/Implementation/Zilog
  ${_libemulation_dir}/Interface/Apple
  ${_libemulation_dir}/Interface/EIA
  ${_libemulation_dir}/Interface/Generic
  ${_libemulation_dir}/Interface/Host
  ${_libemulation_dir}/Interface/IEEE
)