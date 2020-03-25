#include <iostream>

// Reverses the bits of a number
// Example input:
// 13           | 0x1101
//
// Output:
// 11           | 0x1011

unsigned int getValuableBitCount(unsigned int uiNumber) {
  unsigned int uiValuableBitCount = 0;
  while (uiNumber > 0) {
    uiNumber >>= 1;
    uiValuableBitCount++;
  }

  return uiValuableBitCount;
}

void setBitToOne(unsigned int &uiNumber, unsigned int uiBitNumber) {
  uiNumber |= (1 << uiBitNumber);
}

unsigned int reverseBits(unsigned int uiNumberToReverse) {
  unsigned int uiValuableBits = getValuableBitCount(uiNumberToReverse);
  unsigned int uiRevertedNumber = 0, uiActualBitNumber;

  for (uiActualBitNumber = 0; uiActualBitNumber < uiValuableBits; uiActualBitNumber++) {
    unsigned int uiActualBitValue = (uiNumberToReverse & (1 << uiActualBitNumber));
    if (uiActualBitValue)
      setBitToOne(uiRevertedNumber, (uiValuableBits - 1) - uiActualBitNumber);
  }

  return uiRevertedNumber;
}

int main() {
  unsigned int uiNumberToReverse;
  std::cin >> uiNumberToReverse;
  std::cout << reverseBits(uiNumberToReverse);
  return 0;
}