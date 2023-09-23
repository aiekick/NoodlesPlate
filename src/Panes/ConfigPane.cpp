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

#include "ConfigPane.h"

#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>
#include <ImGuiPack.h>

#include <ctools/cTools.h>
#include <ctools/Logger.h>

#include <Project/ProjectFile.h>
#include <Profiler/TracyProfiler.h>
#include <PixelDebug/PixelDebug.h>

// SoGLSL
#include <SoGLSL/src/Systems/CameraSystem.h>
#include <SoGLSL/src/Renderer/RenderPack.h>
#include <SoGLSL/src/Systems/GizmoSystem.h>
#include <SoGLSL/src/Systems/SoundSystem.h>
#include <SoGLSL/src/Helper/InterfacePanes.h>

#ifdef USE_VR
#include <SoGLSL/src/VR/Gui/VRGui.h>
#endif

#include <cinttypes>  // printf zu

ConfigPane::ConfigPane() = default;
ConfigPane::~ConfigPane() = default;

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool ConfigPane::Init() {
    return true;
}

void ConfigPane::Unit() {
}

bool ConfigPane::DrawPanes(const uint32_t& vCurrentFrame, PaneFlags& vInOutPaneShown, ImGuiContext* vContextPtr, const std::string& vUserDatas) {

    if (vInOutPaneShown & paneFlag) {
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
        if (ImGui::Begin<PaneFlags>(paneName.c_str(), &vInOutPaneShown, paneFlag, flags)) {
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
            auto win = ImGui::GetCurrentWindowRead();
            if (win->Viewport->Idx != 0)
                flags |= ImGuiWindowFlags_NoResize;  // | ImGuiWindowFlags_NoTitleBar;
            else
                flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
#endif
            if (ProjectFile::Instance()->IsLoaded()) {
            }

            ZoneScopedN("ConfigPane");

            auto const mbPtr = MainBackend::Instance();
            MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();
            mbPtr->NeedRefresh(CameraSystem::Instance()->DrawImGui());
            mbPtr->puPixelDebug.DrawPane(mbPtr->puDisplay_RenderPack, mbPtr->puDisplayQuality, CameraSystem::Instance());
            mbPtr->NeedRefresh(GizmoSystem::Instance()->DrawPane(mbPtr->puCodeTree, mbPtr->puDisplay_RenderPack, mbPtr->puDisplayQuality, CameraSystem::Instance()));
            mbPtr->NeedRefresh(SoundSystem::Instance()->DrawPane(mbPtr->puCodeTree, mbPtr->puDisplay_RenderPack, mbPtr->puDisplayQuality, CameraSystem::Instance()));

#ifdef USE_VR
            VRGui::Instance()->Draw();
#endif

            const auto width = ImGui::GetContentRegionAvail().x - 14.0f;

            ImGui::Separator();

            RenderPackCheckBox(width, mbPtr->puMain_RenderPack);

            if (mbPtr->puShow3DSpace) {
                RenderPackCheckBox(width, mbPtr->pu3dAxis_RenderPack);
                RenderPackCheckBox(width, mbPtr->pu3dGrid_RenderPack);
            }

            if (mbPtr->puShowMesh) {
                RenderPackCheckBox(width, mbPtr->puMesh_RenderPack);
            }

            /*
            if (mbPtr->puShowBPPlanes)
            {
                RenderPackCheckBox(width, mbPtr->puBPPlanes_RenderPack, "Blueprint Planes");
            }
            */

            if (GizmoSystem::Instance()->UseCulling()) {
                RenderPackCheckBox(width, GizmoSystem::Instance()->GetRenderPack());
            }

            for (auto it : mbPtr->puMain_RenderPack->puBuffers) {
                RenderPackCheckBox(-1, it);
            }

            for (auto it : mbPtr->puMain_RenderPack->puSceneBuffers) {
                auto itPtr = it.lock();
                if (itPtr) {
                    RenderPackCheckBox(-1, it, "Scene : " + itPtr->puName);

                    for (auto sit : itPtr->puBuffers) {
                        RenderPackCheckBox(-1, sit);
                    }
                }
            }

            if (MainFrame::Instance()->puShowDebug) {
                InterfacePanes::Instance()->DrawDebugPane(mbPtr->puCodeTree);
            }
        }

        MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

        ImGui::End();
    }

    return false;
}

std::string ConfigPane::getXml(const std::string& vOffset, const std::string& vUserDatas) {
    UNUSED(vOffset);
    UNUSED(vUserDatas);

    std::string str;

    return str;
}

bool ConfigPane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) {
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

bool ConfigPane::RenderPackCheckBox(float vColumnOffset, RenderPackWeak vRenderPack) {
    auto rpPtr = vRenderPack.lock();
    if (rpPtr) {
        return RenderPackCheckBox(vColumnOffset, vRenderPack, rpPtr->puName);
    }

    return false;
}

bool ConfigPane::RenderPackCheckBox(float /*vColumnOffset*/, RenderPackWeak vRenderPack, std::string vRenderPackString) {
    auto res = false;

    auto const mbPtr = MainBackend::Instance();

    auto vis = (mbPtr->puDisplay_RenderPack.lock() == vRenderPack.lock());

    const auto is_Opened = ImGui::CollapsingHeader_CheckBox(vRenderPackString.c_str(), -1.0f, false, true, &vis);

    if (vis) {
        mbPtr->puDisplay_RenderPack = vRenderPack;
        auto displayShaderPtr = mbPtr->puDisplay_RenderPack.lock();
        if (displayShaderPtr) {
            mbPtr->puPreviewBufferId = ct::mini<int>(mbPtr->puPreviewBufferId, displayShaderPtr->puZeroBasedMaxSliceBufferId);
        }
    }

    if (is_Opened) {
        auto rpPtr = vRenderPack.lock();
        if (rpPtr) {
            if (rpPtr->DrawRenderingButtons(vRenderPackString.c_str())) {
                CameraSystem::Instance()->NeedCamChange();
                mbPtr->NeedRefresh(true);
                res = true;
            }
        }
    }

    return res;
}
