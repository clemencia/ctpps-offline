#include "CondFormats/Serialization/interface/Test.h"

#include "../src/headers.h"

int main()
{
  // testSerialization<CTPPSAlignmentCorrections>();
  testSerialization<CTPPSRPPositions>();

  return 0;
}