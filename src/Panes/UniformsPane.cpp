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

#include "UniformsPane.h"

#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>
#include <ImGuiPack.h>
#include <ctools/cTools.h>
#include <ctools/Logger.h>
#include <Project/ProjectFile.h>

// SoGLSL
#include <SoGLSL/src/Profiler/TracyProfiler.h>
#include <SoGLSL/src/Renderer/RenderPack.h>
#include <SoGLSL/src/CodeTree/CodeTree.h>
#include <SoGLSL/src/Systems/GizmoSystem.h>
#include <SoGLSL/src/Systems/TimeLineSystem.h>

#include <cinttypes>  // printf zu

UniformsPane::UniformsPane() = default;
UniformsPane::~UniformsPane() = default;

///////////////////////////////////////////////////////////////////////////////////
//// OVERRIDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool UniformsPane::Init() {
    return true;
}

void UniformsPane::Unit() {
}

bool UniformsPane::DrawPanes(const uint32_t& /*vCurrentFrame*/, bool* vOpened, ImGuiContext* /*vContextPtr*/, void* /*vUserDatas*/) {
    if (vOpened && *vOpened) {
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (ImGui::Begin(GetName().c_str(), vOpened, flags)) {
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
            auto win = ImGui::GetCurrentWindowRead();
            if (win->Viewport->Idx != 0)
                flags |= ImGuiWindowFlags_NoResize;  // | ImGuiWindowFlags_NoTitleBar;
            else
                flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;
#endif
            if (ProjectFile::Instance()->IsLoaded()) {
            }

            ZoneScopedN("UniformsPane");

            MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

            DrawImGuiRenderPackCategory(MainBackend::Instance()->puMain_RenderPack, false, false, MainBackend::Instance()->puScreenSize);

            for (auto it : MainBackend::Instance()->puMain_RenderPack->puBuffers) {
                DrawImGuiRenderPackCategory(it, false, false, MainBackend::Instance()->puScreenSize);
            }

            for (auto it : MainBackend::Instance()->puMain_RenderPack->puSceneBuffers) {
                DrawImGuiRenderPackCategory(it, false, false, MainBackend::Instance()->puScreenSize);

                auto itPtr = it.lock();
                if (itPtr) {
                    for (auto sit : itPtr->puBuffers) {
                        DrawImGuiRenderPackCategory(sit, false, false, MainBackend::Instance()->puScreenSize);
                    }
                }
            }

            if (MainBackend::Instance()->puShow3DSpace) {
                DrawImGuiRenderPackCategory(MainBackend::Instance()->pu3dAxis_RenderPack, false, false, MainBackend::Instance()->puScreenSize);
                DrawImGuiRenderPackCategory(MainBackend::Instance()->pu3dGrid_RenderPack, false, false, MainBackend::Instance()->puScreenSize);
            }

            if (MainBackend::Instance()->puShowMesh) {
                DrawImGuiRenderPackCategory(MainBackend::Instance()->puMesh_RenderPack, false, false, MainBackend::Instance()->puScreenSize);
            }

            if (GizmoSystem::Instance()->UseCulling()) {
                DrawImGuiRenderPackCategory(GizmoSystem::Instance()->GetRenderPack(), false, false, MainBackend::Instance()->puScreenSize);
            }

            if (!MainBackend::Instance()->puCodeTree->puIncludeKeys.empty()) {
                if (ImGui::CollapsingHeader_Button("Included Files", -1, true, 0, false, nullptr)) {
                    MainBackend::Instance()->NeedRefresh(MainBackend::Instance()->puCodeTree->DrawImGuiIncludesUniformWidget(
                        MainBackend::Instance()->puDisplay_RenderPack, SHADER_UNIFORM_FIRST_COLUMN_WIDTH, MainBackend::Instance()->puScreenSize));
                }
            }
        }

        MainFrame::sAnyWindowsHovered |= ImGui::IsWindowHovered();

        ImGui::End();
    }

    return false;
}

std::string UniformsPane::getXml(const std::string& vOffset, const std::string& vUserDatas) {
    UNUSED(vOffset);
    UNUSED(vUserDatas);

    std::string str;

    return str;
}

bool UniformsPane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) {
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

void UniformsPane::DrawImGuiRenderPackCategory(RenderPackWeak vRp, bool vCheckMode, bool /*vShowEditBtn*/, ct::ivec2 vScreenSize) {
    auto rpPtr = vRp.lock();
    if (rpPtr && rpPtr->GetShaderKey() && rpPtr->GetShaderKey()->puShaderGlobalSettings.showFlag) {
        if (!vCheckMode) {
            // bool showCat = false;
            auto editCatched = false;

            if (rpPtr->CollapsingHeader(rpPtr->puName.c_str(), false, true, &editCatched)) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", rpPtr->GetShaderKey()->puKey.c_str());

                ImGui::PushID(ImGui::IncPUSHID());
                ImGui::Checkbox("Show UnUsed", &rpPtr->GetShaderKey()->puShaderGlobalSettings.showUnUsedUniforms);
                ImGui::PopID();

                if (rpPtr->puName == "3d Axis") {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "X");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Y");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "Z");
                }

                ImGui::Separator();

                MainBackend::Instance()->NeedRefresh(rpPtr->DrawImGuiUniformWidget(SHADER_UNIFORM_FIRST_COLUMN_WIDTH));
            }

            if (editCatched) {
                ImGui::OpenPopup("EditCollapsingHeader");
                CodeTree::puShaderKeyToEditPopup = rpPtr->GetShaderKey();
                CodeTree::puShaderKeyWhereCreateUniformsConfig = nullptr;

                // OpenShader(vRp->GetShaderKey()->puKey);
            }

            MainBackend::Instance()->NeedRefresh(MainBackend::Instance()->puCodeTree->DrawPopups(vRp));
            MainBackend::Instance()->NeedRefresh(MainBackend::Instance()->puCodeTree->DrawDialogs(vRp, vScreenSize));
        }
    }
}

void UniformsPane::DrawImGuiUniformWidget(UniformsMultiLoc* vUniLoc) {
    if (vUniLoc != nullptr) {
        MainBackend::Instance()->NeedRefresh(//
            MainBackend::Instance()->puCodeTree->DrawImGuiUniformWidgetForPanes(//
                vUniLoc->uniform, ImGui::GetContentRegionAvail().x, SHADER_UNIFORM_FIRST_COLUMN_WIDTH));
    }
}