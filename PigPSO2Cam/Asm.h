/*
=======================================================================

   AIRRIDE's C++ (Win32API) Asm Code Library v2.11 (2013/12/29)

=======================================================================
*/
#pragma once

#ifndef __Asm_H__
#define __Asm_H__

#include<Windows.h>

//#pragma comment(lib, "lib/asm.lib")

#define Asm_Byte_Code_Max 0x180

namespace Asm {
	DWORD Write_Hook(char code[], DWORD Prev, DWORD Next, int nop_count = 0);
	DWORD Write_Pointer_Hook(DWORD Pointer, DWORD HookAddr);
	BOOL Write_code(DWORD Addr, char Array[], int nop_count = 0);
	BOOL BytetoChar(unsigned char Packet[], int Size, char StrPacket[]);
	int ChartoByte(char Array[], BYTE b[]);
	int ChartoAob(const char Array[], BYTE b[], BOOL m[]);
	DWORD AobScan(const char Array[], DWORD Memory_Start = 0x00401000, DWORD Memory_End = 0x06000000);
	byte* FindPattern(char* pbMask, const char* pszMask, size_t nLength, byte* pBaseAddress = (BYTE*)(0x00401000));
	void ErrorMessage(const char message[]);

	void writeJmpHook(DWORD dwAddy, LPVOID dwHook, UINT nNops);
}

#endif
