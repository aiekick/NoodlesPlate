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

#include "ProfilerPane.h"

#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>
#include <ImGuiPack.h>
#include <iagp/iagp.h>
#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <Helper/InterfacePanes.h>
#include <CodeTree/CodeTree.h>

#include <Project/ProjectFile.h>

#include <cinttypes> // printf zu

ProfilerPane::ProfilerPane() = default;
ProfilerPane::~ProfilerPane() = default;

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool ProfilerPane::Init()
{
    /*iagp::InAppGpuProfiler::Instance()->SetImGuiBeginFunctor([this](const char* vLabel, bool* pOpen, ImGuiWindowFlags vFlags) -> bool {
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
        if (ImGui::Begin<LayoutPaneFlag>(paneName.c_str(), &m_InOutPaneShown, paneFlag, flags, pOpen)) {
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
            auto win = ImGui::GetCurrentWindowRead();
            if (win->Viewport->Idx != 0)
                flags |= ImGuiWindowFlags_NoResize;  // | ImGuiWindowFlags_NoTitleBar;
            else
                flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
#endif
            return true;
        }
        return false;
    });*/
	return true;
}

void ProfilerPane::Unit()
{

}

bool ProfilerPane::DrawPanes(const uint32_t& /*vCurrentFrame*/, bool* vOpened, ImGuiContext* /*vContextPtr*/, void* /*vUserDatas*/)
{
	iagp::InAppGpuProfiler::Instance()->sIsActive = false;

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
            // draw iagp frame
            iagp::InAppGpuProfiler::Instance()->sIsActive = true;
			iagp::InAppGpuProfiler::Instance()->DrawFlamGraphNoWin();
		}

		MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

		ImGui::End();

        iagp::InAppGpuProfiler::Instance()->DrawFlamGraphChilds();

        iagp::InAppGpuProfiler::Instance()->DrawDetails();

	}

	return false;
}

std::string ProfilerPane::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vOffset);
	UNUSED(vUserDatas);

	std::string str;

	return str;
}

bool ProfilerPane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

