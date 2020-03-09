#pragma once
#include "d3d9.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include <string>
#include <sstream>
#include "../Camera.h"

#include "../PSO2CameraTool.hpp"


typedef HRESULT(__stdcall* pso2hDoLua)(const char* a1);
static pso2hDoLua _executeLua = 0;

bool runLua(std::string a1);

bool initLuaHook();

bool runLuaAsync(std::string a1);

void menu_init(void*, LPDIRECT3DDEVICE9);

static int currTab = 0;

static bool farCullDisabled = false;
static bool farCullObjectsDisabled = false;
static bool nearCullDisabled = false;
static bool freeCamToggle = false;

static DWORD oNearCullBytes = 0x0;


static bool pushCamera()
{
	return runLuaAsync("Camera.PushActorTest()");
}

static bool adjustZoom() {
	std::stringstream ss;
	ss << "Camera.ActorTrackTestNormal.Offset.Dist = \"" << Camera::cameraBase.Offset.Dist << "\"";
	std::string dist(ss.str());
	runLuaAsync(dist.c_str());
	pushCamera();

	return true;
}
static const char* adjustDistmin(float f) {
	std::stringstream ss;
	ss << "Camera.ActorTrackTestNormal.DistMin = " << f;
	return ss.str().c_str();
}
static const char* adjustDistmax(float f) {
	std::stringstream ss;
	ss << "Camera.ActorTrackTestNormal.DistMax = " << f;
	return ss.str().c_str();
}
static bool adjustFovy() {
	std::stringstream ss;
	ss << "Camera.ActorTrackTestNormal.Fovy = " << Camera::cameraBase.Fovy;
	std::string dist(ss.str());
	runLuaAsync(dist.c_str());
	pushCamera();

	return true;
}

static void draw_menu(bool* status)
{

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::GetIO().MouseDrawCursor = true;

	ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(480.0f, 300.f), ImGuiCond_Appearing);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;

	ImGui::Begin("PIG PSO2", status, window_flags);


	switch (currTab) {
	case 0:
	{
		ImGui::BeginGroup();

		{
			ImGui::Columns(2);

			ImGui::Text(("Zoom "));
			ImGui::SameLine(50.0, ImGui::GetStyle().ItemSpacing.y);
			ImGui::SliderInt(("##zoomslider"), &Camera::cameraBase.Offset.Dist, 6, 14);
			ImGui::Text(("Fovy "));
			ImGui::SameLine(50.0, ImGui::GetStyle().ItemSpacing.y);
			ImGui::SliderInt(("##fovslider"), &Camera::cameraBase.Fovy, 40, 58);
			ImGui::Text(("DistMin "));
			ImGui::SameLine(50.0, ImGui::GetStyle().ItemSpacing.y);
			ImGui::SliderInt(("##distminlider"), &Camera::cameraBase.Offset.DistMin, 4, 10);
			ImGui::Text(("DistMax "));
			ImGui::SameLine(50.0, ImGui::GetStyle().ItemSpacing.y);
			ImGui::SliderInt(("##distmaxlider"), &Camera::cameraBase.Offset.DistMax, 30, 300);

			
			
			ImGui::NextColumn();
			if (ImGui::Button("Set Zoom", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
				adjustZoom();
			}
			if (ImGui::Button("Set FOV", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
				adjustFovy();
			}
			//Honestly not quite sure what these two are, figured for culling but idk
			if (ImGui::Button("Set DistMin", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
				runLuaAsync(adjustDistmin((float)Camera::cameraBase.Offset.DistMin));
				pushCamera();
			}
			if (ImGui::Button("Set DistMax", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
				runLuaAsync(adjustDistmax((float)Camera::cameraBase.Offset.DistMax));
				pushCamera();
			}
			ImGui::EndColumns();

			ImGui::Separator();

			ImGui::Columns(2);
			ImGui::Text("Disable Terrain Far Culling ");
			ImGui::Spacing();//meme alignment
			ImGui::Text("Disable Object Far Culling ");
			ImGui::Spacing();
			ImGui::Text("Disable Object Near Culling ");
			ImGui::Spacing();
			ImGui::NextColumn();
			if (ImGui::Checkbox("##farcullcheck", &farCullDisabled))
			{
				BYTE b = 0x76; //original conditional jmp byte
				if (farCullDisabled)
					b = 0xEB;	//unconditional jmp

				uintptr_t tFarCull = getTerrainFarCullAddy();
				if (tFarCull)
					*(BYTE*)(tFarCull) = b;
			}
			
			if (ImGui::Checkbox("##farcullcheckobjects", &farCullObjectsDisabled))
			{
				uintptr_t tFarCull = getObjectFarCullAddy();
				if (tFarCull)
				{
					BYTE b = 0x0F; 
					if (farCullObjectsDisabled) 
					{
						b = 0xE9;	

						*(BYTE*)(tFarCull) = 0x90;
						*(BYTE*)(tFarCull+1) = b;
					}
					else
					{
						*(BYTE*)(tFarCull) = b;
						*(BYTE*)(tFarCull + 1) = 0x84;
					}
				}
			}

			if (ImGui::Checkbox("##nearcullcheck", &nearCullDisabled))
			{
				uintptr_t tNearCull = getCameraNearCullAddy();
				if (tNearCull) {
					
					if (oNearCullBytes == 0x0)
						oNearCullBytes = *(DWORD*)(tNearCull);

					//meme
					if (nearCullDisabled)
						*(DWORD*)(tNearCull) = (DWORD)(0x90909090);
					else
						*(DWORD*)(tNearCull) = (DWORD)(oNearCullBytes);
				}
			}
		/*	if (ImGui::Button("ResetParam", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
				resetParam();
			}*/
			ImGui::EndColumns();
			ImGui::Separator();

			
			ImGui::Text("Ghost Ride da Whip");
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Will break after some patch eventually");
			}
			ImGui::SameLine(0.0, ImGui::GetStyle().ItemSpacing.y);
			if (ImGui::Checkbox("##freecamcheck", &freeCamToggle))
			{
				DWORD funcStart = 0x04FD39D0;
				if (!freeCamToggle || *(BYTE*)(funcStart) == 0x55)
				{
					if (freeCamToggle)
					{
						*(BYTE*)(funcStart) = 0x31;		//xor eax,eax ret 0008
						*(BYTE*)(funcStart + 1) = 0xC0;
						*(BYTE*)(funcStart + 2) = 0xC2;
						*(BYTE*)(funcStart + 3) = 0x08;
						*(BYTE*)(funcStart + 4) = 0x00;
					}
					else
					{
						*(BYTE*)(funcStart) = 0x55;	   //restore entrance
						*(BYTE*)(funcStart + 1) = 0x8B;
						*(BYTE*)(funcStart + 2) = 0xEC;
						*(BYTE*)(funcStart + 3) = 0x83;
						*(BYTE*)(funcStart + 4) = 0xE4;
					}
				}
			}
		}
		ImGui::EndGroup();
		break;
		}
	}

	std::string pig("pig");
	ImVec2 txt = ImGui::CalcTextSize(pig.c_str());
	ImGui::SetCursorPosY(ImGui::GetWindowSize().y * .90f);
	//ImGui::SetCursorPosY(ImGui::GetContentRegionAvail().y * 0.94f);
	ImGui::Separator();

	ImGui::SetCursorPosX(ImGui::GetContentRegionAvailWidth() * 0.49f);
	ImGui::TextColored(ImVec4(0.95f, .36f, .03f, 0.95f), pig.c_str());

	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

}