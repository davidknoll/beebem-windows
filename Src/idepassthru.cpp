/* 16-bit IDE host passthru support for BeebEm, by David Knoll */

#include "idepassthru.h"

bool IDEPassThruEnabled = false;

void PTWrite(int addr, int data) { return; }
int PTRead(int addr) { return 0xFF; }
void PTReset(void) { return; }
