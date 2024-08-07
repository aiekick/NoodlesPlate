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

#include "ConfigSwitcherPane.h"

#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>
#include <ImGuiPack.h>

#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <Renderer/RenderPack.h>
#include <SoGLSL/src/Buffer/FrameBuffer.h>
#include <SoGLSL/src/Gui/CustomGuiWidgets.h>

#include <Project/ProjectFile.h>
#include <Gui/MainToolBar.h>

#include <cinttypes> // printf zu

ConfigSwitcherPane::ConfigSwitcherPane() = default;
ConfigSwitcherPane::~ConfigSwitcherPane() = default;

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool ConfigSwitcherPane::Init()
{
	return true;
}

void ConfigSwitcherPane::Unit()
{

}

bool ConfigSwitcherPane::DrawPanes(const uint32_t& /*vCurrentFrame*/, bool* vOpened, ImGuiContext* /*vContextPtr*/, void* /*vUserDatas*/)
{
	if (vOpened && *vOpened) {
        static ImGuiWindowFlags flags =               //
            ImGuiWindowFlags_NoCollapse |             //
            ImGuiWindowFlags_NoBringToFrontOnFocus |  //
            ImGuiWindowFlags_NoScrollbar |            //
            ImGuiWindowFlags_MenuBar;                 //
        if (ImGui::Begin(GetName().c_str(), vOpened, flags))
		{
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
			auto win = ImGui::GetCurrentWindowRead();
			if (win->Viewport->Idx != 0)
				flags |= ImGuiWindowFlags_NoResize;// | ImGuiWindowFlags_NoTitleBar;
			else
                flags = ImGuiWindowFlags_NoCollapse |             //
                        ImGuiWindowFlags_NoBringToFrontOnFocus |  //
                        ImGuiWindowFlags_NoScrollbar |            //
                        ImGuiWindowFlags_MenuBar;                 //
#endif
			if (ProjectFile::Instance()->IsLoaded()){}

			ShaderKeyConfigSwitcherUnified::Instance()->Activate(
				true,
				MainBackend::Instance()->puCodeTree,
				MainBackend::Instance()->puMain_RenderPack);

			MainBackend::Instance()->NeedRefresh(
				ShaderKeyConfigSwitcherUnified::Instance()->DrawConfigSwitcher(
					MainBackend::Instance()->puCodeTree, MainBackend::Instance()->puMain_RenderPack, MainToolBar::puFont));
		}

		MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

		ImGui::End();
	}
	else
	{
		ShaderKeyConfigSwitcherUnified::Instance()->Activate(false);
	}

	return false;
}

bool ConfigSwitcherPane::DrawDialogsAndPopups(const uint32_t& /*vCurrentFrame*/, const ImRect& /*vMaxRect*/, ImGuiContext* /*vContextPtr*/, void* /*vUserDatas*/) {
    ShaderKeyConfigSwitcherUnified::Instance()->DrawDialog();
    return false;
}

std::string ConfigSwitcherPane::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vOffset);
	UNUSED(vUserDatas);

	std::string str;

	return str;
}

bool ConfigSwitcherPane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

	return true;
}
