// NoodlesPlate Copyright (C) 2017-2024 Stephane Cuillerdier aka Aiekick
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

#include "TimeLinePane.h"

#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>
#include <ImGuiPack.h>
#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <Project/ProjectFile.h>

// SoGLSL
#include <SoGLSL/src/Systems/TimeLineSystem.h>
#include <SoGLSL/src/Renderer/RenderPack.h>

#include <cinttypes> // printf zu

static int GeneratorPaneWidgetId = 0;

TimeLinePane::TimeLinePane() = default;
TimeLinePane::~TimeLinePane() = default;

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool TimeLinePane::Init()
{
	return true;
}

void TimeLinePane::Unit()
{

}

bool TimeLinePane::DrawPanes(const uint32_t& /*vCurrentFrame*/, bool* vOpened, ImGuiContext* /*vContextPtr*/, void* /*vUserDatas*/)
{
	TimeLineSystem::Instance()->SetActive(false);

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

			TimeLineSystem::Instance()->SetActive(true);

			auto change = false;

			if (!MainBackend::Instance()->puDisplay_RenderPack.expired())
			{
				auto displayShaderPtr = MainBackend::Instance()->puDisplay_RenderPack.lock();
				if (displayShaderPtr)
				{
					const auto activeKey = displayShaderPtr->GetShaderKey();
					TimeLineSystem::Instance()->SetActiveKey(activeKey); 
					
					change |= TimeLineSystem::Instance()->DrawBar(activeKey, MainBackend::Instance()->puScreenSize);
					change |= TimeLineSystem::Instance()->DrawTimeLine("TimeLine##customui", activeKey);

					if (ImGui::IsKeyReleased(GuiBackend_KEY_DELETE))
					{
						TimeLineSystem::Instance()->DelSelectedKeys(activeKey);
					}
					MainBackend::Instance()->NeedRefresh(change);
				}
			}
		}
		MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

		ImGui::End();
	}

	return false;
}

std::string TimeLinePane::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vOffset);
	UNUSED(vUserDatas);

	std::string str;

	return str;
}

bool TimeLinePane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

