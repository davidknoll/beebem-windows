/* 16-bit IDE host passthru support for BeebEm, by David Knoll */

#include <windows.h>
#include "idepassthru.h"
#define IDE_CS0 0x170
#define IDE_CS1 0x370

static void (__stdcall *pDlPortWritePortUshort)(USHORT, USHORT);
static USHORT (__stdcall *pDlPortReadPortUshort)(USHORT);
static BOOL (__stdcall *pIsInpOutDriverOpen)(void);

bool IDEPassThruEnabled = false;

void PTWrite(int addr, int data)
{
	static unsigned char outlatch;
	USHORT outptmp;
	if (!pIsInpOutDriverOpen) return;
	if (!(*pIsInpOutDriverOpen)()) return;

	if (addr & 0x8) {
		if (addr & 0x4) {
			if (addr & 0x2) {
				// IDE /CS1
				(*pDlPortWritePortUshort)(IDE_CS1 | (addr & 0x7), data & 0xFF);
			}
		} else {
			// '646 latch
			outlatch = data & 0xFF;
		}
	} else {
		// IDE /CS0
		outptmp = (outlatch << 8) | (data & 0xFF);
		(*pDlPortWritePortUshort)(IDE_CS0 | (addr & 0x7), outptmp);
	}
}

int PTRead(int addr)
{
	static unsigned char inlatch;
	USHORT inptmp;
	if (!pIsInpOutDriverOpen) return 0xFF;
	if (!(*pIsInpOutDriverOpen)()) return 0xFF;

	if (addr & 0x8) {
		if (addr & 0x4) {
			if (addr & 0x2) {
				// IDE /CS1
				return (*pDlPortReadPortUshort)(IDE_CS1 | (addr & 0x7)) & 0xFF;
			} else {
				return 0xFF;
			}
		} else {
			// '646 latch
			return inlatch;
		}
	} else {
		// IDE /CS0
		inptmp = (*pDlPortReadPortUshort)(IDE_CS0 | (addr & 0x7));
		inlatch = (inptmp >> 8) & 0xFF;
		return inptmp & 0xFF;
	}
}

void PTReset(void)
{
	HINSTANCE hInpOutDll = LoadLibrary("inpout32.dll");
	if (hInpOutDll) {
		pDlPortWritePortUshort = (void (__stdcall *)(USHORT, USHORT)) GetProcAddress(hInpOutDll, "DlPortWritePortUshort");
		pDlPortReadPortUshort = (USHORT (__stdcall *)(USHORT)) GetProcAddress(hInpOutDll, "DlPortReadPortUshort");
		pIsInpOutDriverOpen = (BOOL (__stdcall *)(void)) GetProcAddress(hInpOutDll, "IsInpOutDriverOpen");
	}
}
