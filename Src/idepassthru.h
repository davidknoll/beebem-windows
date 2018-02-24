/* 16-bit IDE host passthru support for BeebEm, by David Knoll */

#ifndef PT_HEADER
#define PT_HEADER
extern bool IDEPassThruEnabled;
void PTWrite(int addr, int data);
int PTRead(int addr);
void PTReset(void);
#endif
