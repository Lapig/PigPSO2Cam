#pragma once
#include <Windows.h>


DWORD WINAPI HookThread();
int Initialize();

uintptr_t getTerrainFarCullAddy();
uintptr_t getObjectFarCullAddy();
uintptr_t getCameraNearCullAddy();
static const char* cameraFarCullAob = "76 ?? F2 0F ?? ?? F2 0F ?? ?? 66 0F ?? ?? 66 0F";
static const char* cameraFarCullObjectsAob = "F7 46 ?? 00 00 00 20 0F ?? ?? 02 00 00 8B ?? ??";	// +0x7
static const char* cameraNearCullAob = "F3 0F 5C DA 66 0F EF ?? F3 0F 10 ?? 66 0F EF ??";
