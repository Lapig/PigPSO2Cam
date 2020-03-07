#include"Asm.h"

namespace Asm {
	void ErrorMessage(const char message[]);
	BOOL Enable_Write_Memory(DWORD Addr, int Size);
}

namespace Asm {
	char StrArray[128] = "0123456789ABCDEF";
}

byte* Asm::FindPattern(char* pbMask, const char* pszMask, size_t nLength, byte* pBaseAddress)
{
	auto DataCompare = [](const byte* pData, const byte* mask, const char* cmask, byte chLast, size_t iEnd) -> bool {
		if (pData[iEnd] != chLast) return false;
		for (; *cmask; ++cmask, ++pData, ++mask) {
			if (*cmask == 'x' && *pData != *mask) {
				return false;
			}
		}

		return true;
	};

	auto iEnd = strlen(pszMask) - 1;
	auto chLast = pbMask[iEnd];

	auto* pEnd = pBaseAddress + nLength - strlen(pszMask);
	for (; pBaseAddress < pEnd; ++pBaseAddress) {
		if (DataCompare(pBaseAddress, (const byte*)pbMask, pszMask, chLast, iEnd)) {
			return pBaseAddress;
		}
	}

	return nullptr;
}

BOOL Asm::BytetoChar(BYTE Packet[], int Size, char StrPacket[]) {
	int i, p;

	p = 0;

	if (Size < 2) {
		return FALSE;
	}

	for (i = 0; i < Size; i++) {
		StrPacket[p] = StrArray[Packet[i] / 0x10]; p++;
		StrPacket[p] = StrArray[Packet[i] % 0x10]; p++;
		StrPacket[p] = 0x20; p++;
	}
	StrPacket[p - 1] = 0x00;

	return TRUE;
}

void Asm::ErrorMessage(const char message[]) {
	MessageBoxA(NULL, message, "PigError", NULL);
}

BOOL Asm::Enable_Write_Memory(DWORD Addr, int Size) {
	DWORD old;
	if (VirtualProtect((DWORD*)Addr, Size, PAGE_EXECUTE_READWRITE, &old) == FALSE) {
		ErrorMessage(R"(Enable_Write_Memory)");
		return FALSE;
	}

	return TRUE;
}

DWORD Asm::Write_Hook(char code[], DWORD Prev, DWORD Next, int nop_count) {
	int i;
	BOOL Flag = FALSE;

	if (Enable_Write_Memory(Prev, 7 + nop_count) == FALSE) {
		return FALSE;
	}

	switch (*(DWORD*)code) {
	case 0x00706D6A://jmp
		*(BYTE*)Prev = 0xE9;
		break;

	case 0x6C6C6163://call
		*(BYTE*)Prev = 0xE8;
		break;

	case 0x0000656A://je
		*(WORD*)Prev = 0x840F;
		Flag = TRUE;
		break;

	case 0x00656E6A://jne
		*(WORD*)Prev = 0x850F;
		Flag = TRUE;
		break;

	case 0x0000626A://jb
		*(WORD*)Prev = 0x820F;
		Flag = TRUE;
		break;

	case 0x0000616A://ja
		*(WORD*)Prev = 0x870F;
		Flag = TRUE;
		break;

	default:
		ErrorMessage(R"(@Write_Hook)");
		return FALSE;
		break;
	}

	*(DWORD*)(Prev + 1 + Flag) = Next - Prev - 5 - Flag;

	if (nop_count == 0) {
		return Prev + 5 + Flag;
	}

	for (i = 0; i < nop_count; i++) {
		*(BYTE*)(Prev + 5 + Flag + i) = 0x90;//nop
	}

	return Prev + 5 + nop_count + Flag;
}

DWORD Asm::Write_Pointer_Hook(DWORD Pointer, DWORD HookAddr) {
	DWORD Function;

	if (Enable_Write_Memory(Pointer, 4) == FALSE) {
		return FALSE;
	}

	Function = *(DWORD*)Pointer;

	if (Function == 0) {
		ErrorMessage(R"(@Write_Pointer_Hook)");
		return FALSE;
	}

	*(DWORD*)Pointer = HookAddr;
	return Function;
}

BOOL Asm::Write_code(DWORD Addr, char Array[], int nop_count) {
	int i, Hit;
	BYTE ByteCode[Asm_Byte_Code_Max];//

	if (Array != NULL && Array[0] != 0x00) {
		Hit = Asm::ChartoByte(Array, ByteCode);

		if (Enable_Write_Memory(Addr, Hit) == FALSE) {
			return FALSE;
		}

		for (i = 0; i < Hit; i++) {
			*(BYTE*)(Addr + i) = ByteCode[i];
		}

		if (nop_count == 0) {
			return TRUE;
		}

		if (Enable_Write_Memory(Addr + Hit, nop_count) == FALSE) {
			return FALSE;
		}
	}
	else {
		Hit = 0;
	}

	for (i = 0; i < nop_count; i++) {
		*(BYTE*)(Addr + Hit + i) = 0x90;//nop
	}

	return TRUE;
}

int Asm::ChartoByte(char Array[], BYTE b[]) {
	int i, Hit = 0;
	for (i = 0; Array[i] != 0x00; i++) {
		if (Array[i] == 0x2A) { Array[i] = StrArray[rand() % 0x10]; }
		if (Array[i] >= 0x61 && Array[i] <= 0x66) { b[Hit] = Array[i] - 0x57; Hit++; }
		else if (Array[i] >= 0x41 && Array[i] <= 0x46) { b[Hit] = Array[i] - 0x37; Hit++; }
		else if (Array[i] >= 0x30 && Array[i] <= 0x39) { b[Hit] = Array[i] - 0x30; Hit++; }
	}

	if (Hit % 2 != 0) {
		ErrorMessage(R"(@ChartoByte)");
		return -1;
	}

	Hit = Hit / 2;

	for (i = 0; i < Hit; i++) {
		b[i] = b[i * 2] * 0x10 + b[i * 2 + 1];
	}

	return Hit;
}

int Asm::ChartoAob(const char Array[], BYTE b[], BOOL m[]) {
	int i, Hit = 0;
	for (i = 0; Array[i] != 0x00; i++) {
		if (Array[i] == 0x3F) { b[Hit] = 0x00; m[Hit / 2] = TRUE; Hit++; continue; }
		if (Array[i] >= 0x61 && Array[i] <= 0x66) { b[Hit] = Array[i] - 0x57; Hit++; }
		else if (Array[i] >= 0x41 && Array[i] <= 0x46) { b[Hit] = Array[i] - 0x37; Hit++; }
		else if (Array[i] >= 0x30 && Array[i] <= 0x39) { b[Hit] = Array[i] - 0x30; Hit++; }
	}

	if (Hit % 2 != 0) {
		ErrorMessage(R"(@ChartoByte)");
		return -1;
	}

	Hit = Hit / 2;

	for (i = 0; i < Hit; i++) {
		b[i] = b[i * 2] * 0x10 + b[i * 2 + 1];
	}

	return Hit;
}

DWORD Asm::AobScan(const char Array[], DWORD Memory_Start, DWORD Memory_End) {
	BYTE ByteCode[Asm_Byte_Code_Max];
	BOOL Mask[Asm_Byte_Code_Max] = { 0 };
	int hit, i;
	DWORD MS_Memory;
	hit = ChartoAob(Array, ByteCode, Mask);

	__try {
		for (MS_Memory = Memory_Start; MS_Memory < Memory_End; MS_Memory++) {
			for (i = 0; i < hit; i++) {
				if (Mask[i] == TRUE) {
					continue;
				}
				if (*(BYTE*)(MS_Memory + i) ^ ByteCode[i]) {
					break;
				}
			}
			if (i == hit) {
				return MS_Memory;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return 0;
	}
	return 0;
}

void Asm::writeJmpHook(DWORD dwAddy, LPVOID dwHook, UINT nNops) {
	if (!dwAddy) {
		return;
	}
	DWORD dwOldProtect;
	VirtualProtect((LPVOID)dwAddy, 5 + nNops, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*(BYTE*)dwAddy = (BYTE)0xE9;
	*(DWORD*)(dwAddy + 1) = (DWORD)(((DWORD)dwHook - (DWORD)dwAddy) - 5);
	FillMemory((LPVOID)(dwAddy + 5), nNops, 0x90);
	VirtualProtect((LPVOID)dwAddy, 5 + nNops, dwOldProtect, &dwOldProtect);
}