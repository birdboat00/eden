#pragma once

#include "types.hh"

namespace edn {
  const str kEdenVersion = "22w42a";
  const u16 kEdenBytecodeVersion = 0x0002;
  #define EDEN_BUILD_TIME __TIME__ " on " __DATE__
  const str kEdenBuildTime = EDEN_BUILD_TIME; 
  #undef EDEN_BUILD_TIME
  const u16 kEdenPackMagic = 0x3d9c;
}