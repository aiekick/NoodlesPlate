// NoodlesPlate Copyright (C) 2017-2023 Stephane Cuillerdier aka Aiekick
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "MessagePane.h"

#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>
#include <ImGuiPack.h>
#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <Helper/InterfacePanes.h>
#include <Systems/GizmoSystem.h>
#include <Renderer/RenderPack.h>
#include <SoGLSL/src/Res/CustomFontToolBar.h>

#include <Project/ProjectFile.h>

#include <cinttypes> // printf zu

MessagePane::MessagePane() = default;
MessagePane::~MessagePane() = default;

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool MessagePane::Init()
{
	return true;
}

void MessagePane::Unit()
{

}

bool MessagePane::DrawPanes(const uint32_t& /*vCurrentFrame*/, bool* vOpened, ImGuiContext* /*vContextPtr*/, void* /*vUserDatas*/)
{
	IsThereSomeMessages(false);

	if ((puIsThereSomeShaderErrors && puShowErrors) || (puIsThereSomeShaderWarnings && puShowWarnings))
	{
        LayoutManager::Instance()->ShowSpecificPane(GetFlag());  // open the message pane
	}
	else
	{
        LayoutManager::Instance()->HideSpecificPane(GetFlag());
	}

	if (vOpened && *vOpened)
	{
		static ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_MenuBar;
        if (ImGui::Begin(GetName().c_str(), vOpened, flags))
		{
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
			auto win = ImGui::GetCurrentWindowRead();
			if (win->Viewport->Idx != 0)
				flags |= ImGuiWindowFlags_NoResize;// | ImGuiWindowFlags_NoTitleBar;
			else
				flags = ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_MenuBar;
#endif
			if (ProjectFile::Instance()->IsLoaded()) {}

			if (ImGui::BeginMenuBar())
			{
				ImGui::MenuItem("Errors", "", &puShowErrors);
				ImGui::MenuItem("Warnings", "", &puShowWarnings);

				ImGui::EndMenuBar();
			}

			const auto codeTree = MainBackend::Instance()->puCodeTree;
			const auto showDebug = MainFrame::Instance()->puShowDebug;
			const auto show3DSpace = MainBackend::Instance()->puShow3DSpace;
			const auto showMesh = MainBackend::Instance()->puShowMesh;
            const auto showCulling = GizmoSystem::Instance()->UseCulling();

			if (puIsThereSomeShaderErrors && puShowErrors)
			{
				ImGui::Separator();
				
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Shaders (GLSL) Errors :");

				InterfacePanes::Instance()->DisplayMessageOfRenderPack(MainBackend::sMainThread, false, codeTree, MainBackend::Instance()->puDisplay_RenderPack, showDebug, true, true);

				if (show3DSpace)
				{
					InterfacePanes::Instance()->DisplayMessageOfRenderPack(MainBackend::sMainThread, false, codeTree, MainBackend::Instance()->pu3dAxis_RenderPack, showDebug, true, true);
					InterfacePanes::Instance()->DisplayMessageOfRenderPack(MainBackend::sMainThread, false, codeTree, MainBackend::Instance()->pu3dGrid_RenderPack, showDebug, true, true);
				}

				if (showMesh)
				{
					InterfacePanes::Instance()->DisplayMessageOfRenderPack(MainBackend::sMainThread, false, codeTree, MainBackend::Instance()->puMesh_RenderPack, showDebug, true, true);
				}

				if (showCulling)
				{
					InterfacePanes::Instance()->DisplayMessageOfRenderPack(MainBackend::sMainThread, false, codeTree, GizmoSystem::Instance()->GetRenderPack(), showDebug, true, true);
                }
			}

			if (puIsThereSomeShaderWarnings && puShowWarnings)
			{
				ImGui::Separator();

				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Shaders (GLSL) Warnings :");

				InterfacePanes::Instance()->DisplayMessageOfRenderPack(MainBackend::sMainThread, false, codeTree, MainBackend::Instance()->puDisplay_RenderPack, showDebug, true, true);

				if (show3DSpace)
				{
					InterfacePanes::Instance()->DisplayMessageOfRenderPack(MainBackend::sMainThread, false, codeTree, MainBackend::Instance()->pu3dAxis_RenderPack, showDebug, true, true);
					InterfacePanes::Instance()->DisplayMessageOfRenderPack(MainBackend::sMainThread, false, codeTree, MainBackend::Instance()->pu3dGrid_RenderPack, showDebug, true, true);
				}

				if (showMesh)
				{
					InterfacePanes::Instance()->DisplayMessageOfRenderPack(MainBackend::sMainThread, false, codeTree, MainBackend::Instance()->puMesh_RenderPack, showDebug, true, true);
				}

				if (showCulling)
				{
					InterfacePanes::Instance()->DisplayMessageOfRenderPack(MainBackend::sMainThread, false, codeTree, GizmoSystem::Instance()->GetRenderPack(), showDebug, true, true);
                }
			}
		}

		MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

		ImGui::End();
	}

	return false;
}

std::string MessagePane::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vOffset);
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + "<showerrors>" + (puShowErrors ? "true" : "false") + "</showerrors>\n";
	str += vOffset + "<showwarnings>" + (puShowWarnings ? "true" : "false") + "</showwarnings>\n";

	return str;
}

bool MessagePane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	// The value of this child identifies the name of this element
	std::string strName;
	std::string strValue;
	std::string strParentName;

	strName = vElem->Value();
	if (vElem->GetText())
		strValue = vElem->GetText();
	if (vParent != nullptr)
		strParentName = vParent->Value();

	if (strName == "showerrors")
		puShowErrors = ct::ivariant(strValue).GetB();

	if (strName == "showwarnings")
		puShowWarnings = ct::ivariant(strValue).GetB();

	return true;
}


///////////////////////////////////////////////////////
//// ERRORS / HELP  ///////////////////////////////////
///////////////////////////////////////////////////////

bool MessagePane::IsThereSomeMessages(bool vCheckOnlyErrors)
{
	const auto show3DSpace = MainBackend::Instance()->puShow3DSpace;
	const auto showMesh = MainBackend::Instance()->puShowMesh;
    const auto showCulling = GizmoSystem::Instance()->UseCulling();

	puIsThereSomeShaderErrors = MainBackend::Instance()->puMain_RenderPack->IsTheseSomeErrors();
	if (show3DSpace)
	{
		puIsThereSomeShaderErrors |= MainBackend::Instance()->pu3dAxis_RenderPack->IsTheseSomeErrors();
		puIsThereSomeShaderErrors |= MainBackend::Instance()->pu3dGrid_RenderPack->IsTheseSomeErrors();
	}
	if (showMesh)
	{
		puIsThereSomeShaderErrors |= MainBackend::Instance()->puMesh_RenderPack->IsTheseSomeErrors();
	}
	if (showCulling)
	{
		auto gzPtr = GizmoSystem::Instance()->GetRenderPack().lock();
		if (gzPtr)
		{
			puIsThereSomeShaderErrors |= gzPtr->IsTheseSomeErrors();
		}
    }

	if (!vCheckOnlyErrors)
	{
		puIsThereSomeShaderWarnings = MainBackend::Instance()->puMain_RenderPack->IsTheseSomeWarnings();
		if (show3DSpace)
		{
			puIsThereSomeShaderWarnings |= MainBackend::Instance()->pu3dAxis_RenderPack->IsTheseSomeWarnings();
			puIsThereSomeShaderWarnings |= MainBackend::Instance()->pu3dGrid_RenderPack->IsTheseSomeWarnings();
		}
		if (showMesh)
		{
			puIsThereSomeShaderWarnings |= MainBackend::Instance()->puMesh_RenderPack->IsTheseSomeWarnings();
		}
		if (showCulling)
		{
			auto gzPtr = GizmoSystem::Instance()->GetRenderPack().lock();
			if (gzPtr)
			{
				puIsThereSomeShaderWarnings |= gzPtr->IsTheseSomeWarnings();
			}
        }
	}

	return puIsThereSomeShaderErrors || (!vCheckOnlyErrors && puIsThereSomeShaderWarnings);
}

void MessagePane::DrawToolBarButtons(float /*vWidth*/, ImFont* vFontSymbol) {
	if (puIsThereSomeShaderErrors)
	{
		ImGui::Separator();

		if (ImGui::CoolBarItem()) {
            const auto aw = ImGui::GetCoolBarItemWidth();
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.9f, 0.9f, 0.9f, 1.0f)); // bg when puShowErrors is false
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.9f, 0.9f, 0.9f, 1.0f)); // bg when puShowErrors is false
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.9f, 0.9f, 0.9f, 1.0f)); // bg when puShowErrors is false
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.1f, 0.1f, 1.0f)); // text color when puShowErrors is false
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.0f)); // button color when puShowErrors is true
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.2f, 0.2f, 1.0f)); // button color when puShowErrors is true
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f)); // button color when puShowErrors is true
			ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // text color when puShowErrors is true
			ImGui::RadioButtonLabeled(ImVec2(aw, aw), ICON_NDPTB_COMMENT_REMOVE "##Errors", "Errors", &puShowErrors, false, vFontSymbol);
			ImGui::PopStyleColor(8);
		}
	}

	if (puIsThereSomeShaderWarnings)
	{
		ImGui::Separator();

		if (ImGui::CoolBarItem()) {
            const auto aw = ImGui::GetCoolBarItemWidth();
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.9f, 0.9f, 0.9f, 1.0f)); // bg when puShowErrors is false
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.9f, 0.9f, 0.9f, 1.0f)); // bg when puShowErrors is false
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.9f, 0.9f, 0.9f, 1.0f)); // bg when puShowErrors is false
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.1f, 1.0f)); // text color when puShowErrors is false
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.3f, 1.0f)); // button color when puShowErrors is true
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.2f, 1.0f)); // button color when puShowErrors is true
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.1f, 1.0f)); // button color when puShowErrors is true
			ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // text color when puShowErrors is true
			ImGui::RadioButtonLabeled(ImVec2(aw, aw), ICON_NDPTB_COMMENT_ALERT "##Warnings", "Warnings", &puShowWarnings, false, vFontSymbol);
			ImGui::PopStyleColor(8);
		}
	}
}