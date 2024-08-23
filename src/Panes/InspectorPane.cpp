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

#include "InspectorPane.h"

#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>
#include <ImGuiPack.h>
#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <Renderer/RenderPack.h>
#include <SoGLSL/src/Buffer/FrameBuffersPipeLine.h>
#include <SoGLSL/src/Buffer/FrameBuffer.h>
#include <SoGLSL/src/Gui/CustomGuiWidgets.h>

#include <Project/ProjectFile.h>

#include <cinttypes> // printf zu

InspectorPane::InspectorPane() = default;
InspectorPane::~InspectorPane() = default;

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool InspectorPane::Init()
{
	return true;
}

void InspectorPane::Unit()
{

}

inline void DrawBufferPipe(RenderPackWeak vRP, const char* vLabel, const float& aw)
{
	auto rpPtr = vRP.lock();
	if (rpPtr)
	{
		ImGui::BeginGroup();
		ImGui::Text("%s : %s", vLabel, rpPtr->puName.c_str());
		auto pipe = rpPtr->GetPipe();
		if (pipe)
		{
			for (auto i = 0; i < pipe->getCountTextures(); i++)
			{
				const auto backId = pipe->getBackFboID();
				ImGui::Text("Back");
				ImGui::TextureOverLay(aw, pipe->getBackTexture(i).get(),
					ImVec4(0, 0, 0, 0), ct::toStr("FBO %u", backId).c_str(), ImVec4(0, 0, 0, 1), ImVec4(0, 0, 0, 0));
				ImGui::Text("Front");
				const auto frontId = pipe->getFrontFboID();
                ImGui::TextureOverLay(aw, pipe->getFrontTexture(i).get(),
					ImVec4(0, 0, 0, 0), ct::toStr("FBO %u", frontId).c_str(), ImVec4(0, 0, 0, 1), ImVec4(0, 0, 0, 0));
			}
		}
		ImGui::EndGroup();
	}
}

inline int DrawBufferChain(RenderPackWeak vRoot, RenderPackWeak vRP, std::set<std::string> vBufferNames, const float& aw)
{
	auto res = 0;

	auto rpPtr = vRP.lock();
	auto rootPtr = vRoot.lock();
	if (rpPtr && rootPtr)
	{
		auto key = rpPtr->GetShaderKey();
		if (key)
		{
			const auto arrPtr = key->GetUniformsByWidget("buffer");
			if (arrPtr)
			{
				for (auto uni : *arrPtr)
				{
					if (uni->glslType == uType::uTypeEnum::U_SAMPLER2D && !uni->bufferShaderName.empty())
					{
						ImGui::PushItemWidth(aw);
						ImGui::BeginGroup();
						ImGui::Text("Uniform : %s", uni->name.c_str());
						ImGui::Text("Target : %s", uni->bufferShaderName.c_str());
						auto rrp = rootPtr->puBuffers.get(uni->bufferShaderName).lock();
						if (rrp && rrp->GetPipe() && rrp->GetPipe()->getCountTextures() > 0)
						{
							const auto backId = rrp->GetPipe()->getBackFboID();
							const auto frontId = rrp->GetPipe()->getFrontFboID();
							ImGui::Text("Back FBO %u", backId);
                            ImGui::TextureOverLay(aw, rrp->GetPipe()->getBackTexture(0).get(),
								ImVec4(0, 0, 0, 0), ct::toStr("FBO %u", backId).c_str(), ImVec4(0, 0, 0, 1), ImVec4(0, 0, 0, 0));
							ImGui::Text("Front FBO %u", frontId);
                            ImGui::TextureOverLay(aw, rrp->GetPipe()->getFrontTexture(0).get(),
								ImVec4(0, 0, 0, 0), ct::toStr("FBO %u", frontId).c_str(), ImVec4(0, 0, 0, 1), ImVec4(0, 0, 0, 0));
						}
						ImGui::EndGroup();
						ImGui::PopItemWidth();

						if (vBufferNames.find(uni->bufferShaderName) == vBufferNames.end()) // nwithout this test we will have an infite loop
						{
							if (rootPtr->puBuffers.exist(uni->bufferShaderName))
							{
								vBufferNames.emplace(uni->bufferShaderName); // for avoid infinite loop

								ImGui::SameLine();

								res = DrawBufferChain(vRoot, rootPtr->puBuffers.get(uni->bufferShaderName), vBufferNames, aw) + 1;
							}
						}
					}
				}
			}
		}
	}

	return res;
}

bool InspectorPane::DrawPanes(const uint32_t& /*vCurrentFrame*/, bool* vOpened, ImGuiContext* /*vContextPtr*/, void* /*vUserDatas*/)
{
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
				ImGui::EndMenuBar();
			}

			const auto mar = MainBackend::Instance()->puMain_RenderPack;
			if (mar)
			{
				auto aw = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().FramePadding.x;
				aw /= (float)(mar->puBuffers.size() + 1);
				aw *= 0.25f;

				DrawBufferPipe(mar, "Main Buffer", aw);

				for (auto sb : mar->puBuffers)
				{
					if (!sb.expired())
					{
						ImGui::SameLine();

						ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

						ImGui::SameLine();

						DrawBufferPipe(sb, "Sub Buffer", aw);
					}
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

				ImGui::Text("Buffers Chain for %s", mar->puName.c_str());

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				
				// need one frame
				static uint32_t countTargets = 1;
				aw = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().FramePadding.x) / (float)countTargets;
				countTargets = DrawBufferChain(mar, mar, std::set<std::string>(), aw) + 1;
			}
		}

		MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

		ImGui::End();
	}

	return false;
}

std::string InspectorPane::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vOffset);
	UNUSED(vUserDatas);

	std::string str;

	return str;
}

bool InspectorPane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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
