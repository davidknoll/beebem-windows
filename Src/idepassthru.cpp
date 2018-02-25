/* 16-bit IDE host passthru support for BeebEm, by David Knoll */

#include <windows.h>
#include "idepassthru.h"

static void (*pOut32)(short, short);
static short (*pInp32)(short);
static bool (*pIsInpOutDriverOpen)(void);

bool IDEPassThruEnabled = false;
static int inlatch, outlatch;

void PTWrite(int addr, int data)
{
	int outptmp;
	if (!pIsInpOutDriverOpen) return;
	if (!(*pIsInpOutDriverOpen)()) return;

	if (addr & 0x8) {
		if (addr & 0x4) {
			// IDE /CS1
			(*pOut32)(0x0370 | (addr & 0x7), data & 0xFF);
		} else {
			// '646 latch
			outlatch = data & 0xFF;
		}
	} else {
		// IDE /CS0
		outptmp = (outlatch << 8) | (data & 0xFF);
		(*pOut32)(0x0170 | (addr & 0x7), outptmp);
	}
}

int PTRead(int addr)
{
	int inptmp;
	if (!pIsInpOutDriverOpen) return 0xFF;
	if (!(*pIsInpOutDriverOpen)()) return 0xFF;

	if (addr & 0x8) {
		if (addr & 0x4) {
			// IDE /CS1
			return (*pInp32)(0x0370 | (addr & 0x7)) & 0xFF;
		} else {
			// '646 latch
			return inlatch;
		}
	} else {
		// IDE /CS0
		inptmp = (*pInp32)(0x0170 | (addr & 0x7));
		inlatch = (inptmp >> 8) & 0xFF;
		return inptmp & 0xFF;
	}
}

void PTReset(void)
{
	HINSTANCE hInpOutDll = LoadLibrary("inpout32.dll");
	if (hInpOutDll) {
		pOut32 = (void (*)(short, short)) GetProcAddress(hInpOutDll, "Out32");
		pInp32 = (short (*)(short)) GetProcAddress(hInpOutDll, "Inp32");
		pIsInpOutDriverOpen = (bool (*)(void)) GetProcAddress(hInpOutDll, "IsInpOutDriverOpen");
	}
}
