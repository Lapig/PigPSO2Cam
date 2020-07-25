#pragma once
#include "dllmain.h"
#include "PSO2CameraTool.hpp"



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&Initialize, 0, 0, 0);
		//Initialize();
		break;
	}
	return TRUE;
}