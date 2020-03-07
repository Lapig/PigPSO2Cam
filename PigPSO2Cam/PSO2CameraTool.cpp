#pragma once
#include "PSO2CameraTool.hpp"

#include <detours.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "Asm.h"
#include "imgui/settings_form.h"

using namespace Asm;

bool m_bCreated = false;
D3DVIEWPORT9 viewport;
LPD3DXFONT dxFont;
HMODULE hmRendDx9Base = NULL;

HWND game_hwnd = 0;
HMODULE psoBase = 0;

typedef HRESULT(__stdcall* tEndScene)(LPDIRECT3DDEVICE9 Device);
tEndScene oEndScene;

typedef HRESULT(__stdcall* tReset)(LPDIRECT3DDEVICE9 Device, D3DPRESENT_PARAMETERS* pPresentationParameters);
tReset oReset;

WNDPROC game_wndproc = NULL;
extern IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static bool MENU_DISPLAYING = false;


static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

uintptr_t cameraFarCullJna;
uintptr_t cameraFarCullObjectJe;
uintptr_t cameraNearCullAddy;


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam) || (MENU_DISPLAYING))
	{
		if (MENU_DISPLAYING && msg == WM_KEYDOWN && wParam==VK_ESCAPE) {
			MENU_DISPLAYING = false;
		}
		return true;
	}

	return CallWindowProc(game_wndproc, hWnd, msg, wParam, lParam);
}

uintptr_t getTerrainFarCullAddy()
{
	return cameraFarCullJna;
}
uintptr_t getObjectFarCullAddy()
{
	return cameraFarCullObjectJe;
}
uintptr_t getCameraNearCullAddy()
{
	return cameraNearCullAddy;
}
HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 Device)
{
	if (Device == nullptr)
		return oEndScene(Device);
	if (!m_bCreated)
	{
		m_bCreated = true;
		D3DXCreateFontA(Device, 20, 0, FW_BOLD, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas", &dxFont);
		Device->GetViewport(&viewport);
		game_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(game_hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc));

		DWORD farCullScan = AobScan(cameraFarCullAob);
		if (farCullScan)
			cameraFarCullJna = farCullScan;
		DWORD objectCullScan = AobScan(cameraFarCullObjectsAob);
		if (objectCullScan) {
			cameraFarCullObjectJe = objectCullScan;
			cameraFarCullObjectJe += 0x7;
		}
		DWORD nearCullScan = AobScan(cameraNearCullAob);
		if (nearCullScan)
			cameraNearCullAddy = nearCullScan;
		
		menu_init(game_hwnd, Device);
	}

	if ((GetAsyncKeyState(0x2D) & 1)) { //insert
		MENU_DISPLAYING = !MENU_DISPLAYING;
	}
	if (MENU_DISPLAYING )
	{
		draw_menu(&MENU_DISPLAYING);
	}


	return oEndScene(Device);
}
HRESULT APIENTRY hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (dxFont)
		dxFont->OnLostDevice();

	HRESULT result = oReset(pDevice, pPresentationParameters);
	if (SUCCEEDED(result))
	{
		if (dxFont)
			dxFont->OnResetDevice();
		m_bCreated = false;
		ImGui_ImplDX9_InvalidateDeviceObjects();
		pDevice->GetViewport(&viewport);

		ImGui_ImplDX9_CreateDeviceObjects();
	}
	return result;
}


DWORD_PTR* pVTable;
bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	pVTable = (DWORD_PTR*)g_pd3dDevice;
	pVTable = (DWORD_PTR*)pVTable[0];
	return true;
}

DWORD WINAPI HookThread()
{

	CreateDeviceD3D(game_hwnd);
	if (!pVTable)
		return false;
	
	oEndScene = (tEndScene)pVTable[42];
	oReset = (tReset)pVTable[16];
	
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(LPVOID&)oEndScene, (PBYTE)hkEndScene);
	DetourAttach(&(LPVOID&)oReset, (PBYTE)hkReset);
	DetourTransactionCommit();

	return 0;
}

BOOL CALLBACK find_game_hwnd(HWND hwnd, LPARAM game_pid) {
	DWORD hwnd_pid = NULL;
	GetWindowThreadProcessId(hwnd, &hwnd_pid);
	if (hwnd_pid != game_pid)
		return TRUE;
	game_hwnd = hwnd;
	return FALSE;
}
void ShowDebugConsole()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());

	char charConsoleName[128];
	sprintf_s(charConsoleName, "PSO2DEBUG | PID: %08X (%d)", GetCurrentProcessId(), GetCurrentProcessId());
	SetConsoleTitleA(charConsoleName);
}
int Initialize() {
	while (hmRendDx9Base == NULL)
	{
		Sleep(100);
		hmRendDx9Base = GetModuleHandleA("d3d9.dll");
	}
	Sleep(5000); //meme to allow tweaker to load dll
	EnumWindows(find_game_hwnd, GetCurrentProcessId());
//	ShowDebugConsole();
	
	
	HookThread();
	return 1;
}