#pragma once
#include "settings_form.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <string>

#include <thread>
#include <iostream>

int str2intbool(std::string s) {
	return (s == "1") ? 1 : 0;
}

void luaPrint(std::string s) {

	std::stringstream ss;
	ss << "file = io.open('debugout.lua', 'a+'); io.output(file); if type("<<s<<")=='string' then io.write(" << s << ") elseif type("<<s<<")=='table' then for key,value in pairs(" << s << ") do io.write(key, ' ', tostring(value), ' ') end else io.write('nil') end; io.write('##'); io.close(file);";
	std::string dist(ss.str());

	runLuaAsync(dist.c_str());
}
/*
void resetParam() {
	const char *a1 = "ResetParam()";
	runLuaAsync(a1);
}*/

bool runLuaAsync(std::string a1) {
	if (initLuaHook()) {
		std::thread luacall(runLua, a1);
		luacall.detach();
		return true;
	}
	return false;
}

bool runLua(std::string a1) {
	const char* cA1 = a1.c_str();
	
	_executeLua(cA1);
	return true;
}

bool initLuaHook() {
	if (_executeLua)
		return true;
	HMODULE pso2h = GetModuleHandleA("pso2h.dll");
	if (!pso2h) {
//		Log("Failed to load pso2h.dll");
		return false;
	}
	_executeLua = (pso2hDoLua)GetProcAddress(pso2h, MAKEINTRESOURCEA(8));	//pso2hDoLua ordinal 
	

	if (_executeLua) {
		return true;
	}
	return false; 
}

void *hwnd;
void menu_init(void *game_hwnd, LPDIRECT3DDEVICE9 Device)
{
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(game_hwnd);
	ImGui_ImplDX9_Init(Device);
	ImGuiIO &io = ImGui::GetIO();
//	io.WantCaptureKeyboard = true;
	io.WantCaptureMouse = true;
	io.Fonts->AddFontDefault();
	io.IniFilename = NULL;
	//io.DisplaySize = ImVec2(vp.Height, vp.Width);
	hwnd = game_hwnd;
	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 1.0;
//	style.ChildWindowRounding = 3;
	style.WindowRounding = 3.0;
	style.GrabRounding = 1;
	style.GrabMinSize = 20;
	style.FrameRounding = 0;
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);//default
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.ItemSpacing = ImVec2(8, 6);
	style.WindowPadding = ImVec2(8, 8);
	style.TabRounding = 1.f;
	style.TabBorderSize *= 2;
	style.FrameBorderSize = 1;
	style.TabBorderSize = 1;

	//ImColor mainColor = ImColor(54, 54, 54, 255);
	//ImColor bodyColor = ImColor(24, 24, 24, 240);

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_ChildWindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.53f, 0.53f, 0.53f, 0.46f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.53f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.2f, 0.65f, 0.17f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.47f, 0.47f, 0.91f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.55f, 0.55f, 0.62f);
	colors[ImGuiCol_Button] = ImVec4(0.50f, 0.50f, 0.50f, 0.63f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.67f, 0.68f, 0.63f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.26f, 0.26f, 0.63f);

/*	colors[ImGuiCol_Header] = ImVec4(0.950f, 0.923f, 0.036f, 0.514f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.928f, 0.864f, 0.041f, 0.451f);

	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);*/
	colors[ImGuiCol_Header] = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);

	colors[ImGuiCol_Separator] = ImVec4(0.58f, 0.58f, 0.58f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.87f, 0.87f, 0.87f, 0.53f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.77f, 0.33f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87f, 0.55f, 0.08f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47f, 0.60f, 0.76f, 0.47f);
	colors[ImGuiCol_Tab] = ImVec4(0.54f, 0.54f, 0.54f, 0.58f);
	colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.64f, 0.65f, 0.65f, 0.80f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.54f, 0.54f, 0.54f, 0.58f);

	colors[ImGuiCol_COUNT] = ImVec4(0.26f, 0.26f, 0.26f, 0.63f);
}
