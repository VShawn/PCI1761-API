#include "PCI1761.h"


int PCI1761::entityCount = 0;
bool PCI1761::hasOpened = false;
InstantDiCtrl * PCI1761::instantDiCtrl = NULL;//Create a 'InstantDiCtrl' for DI function.
InstantDoCtrl * PCI1761::instantDoCtrl = NULL;//Create a instantDoCtrl for DO function.


byte PCI1761::lastSignalIDI_RisingEdge = 0;
byte PCI1761::lastSignalIDI_TrailingEdge = 0;

byte PCI1761::bufferForR = 0;