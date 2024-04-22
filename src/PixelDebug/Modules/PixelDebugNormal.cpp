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

// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "PixelDebugNormal.h"
#include <ImGuiPack.h>

#include <SoGLSL/src/Gui/CustomGuiWidgets.h>
#include <SoGLSL/src/Renderer/RenderPack.h>
#include <SoGLSL/src/Systems/CameraSystem.h>

#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>

PixelDebugNormal::PixelDebugNormal() {
    pulastParsedAttachmentId = -1;
    puNeedRefresh = false;

    puAttachment_DF = -1;
    puAttachment_Normal = -1;
    puNormalLength = 0.2f;

    puUse3D = false;
    puActivated = false;
}

PixelDebugNormal::~PixelDebugNormal() {
}

void PixelDebugNormal::Init(RenderPackWeak /*vRenderPack*/) {
    puActivated = false;
}

void PixelDebugNormal::DrawMenu() {
    ImGui::Checkbox("Debug 2D/3D Normals", &puActivated);
}

void PixelDebugNormal::DrawTooltips(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera) {
    if (!puActivated)
        return;

    auto rpPtr = vRenderPack.lock();
    if (rpPtr && rpPtr->GetPipe() && MainFrame::sCentralWindowHovered) {
        const auto mi = MainBackend::Instance()->puMouseInterface;
        const ct::fvec2 mousePos(mi.px, mi.py);
        auto pos = mousePos / vDisplayQuality;
        if (rpPtr->puMeshRect.IsContainPoint(pos)) {
            puNeedRefresh = true;

            if (puAttachment_DF > -1) {
            }

            if (puAttachment_Normal > -1) {
                if (puUse3D && vCamera) {
                    pos.x = (pos.x - rpPtr->puMeshRect.x) / rpPtr->puMeshRect.w;
                    pos.y = 1.0f - (pos.y - rpPtr->puMeshRect.y) / rpPtr->puMeshRect.h;

                    auto mo = vCamera->uModel;
                    mo[3].x = 0.0f;
                    mo[3].y = 0.0f;
                    mo[3].z = 0.0f;

                    auto m = vCamera->uView * mo;

                    const auto n = puValues[puAttachment_Normal].xyz();

                    auto p = ImVec2(pos.x * rpPtr->puMeshRect.w + rpPtr->puMeshRect.x, (1.0f - pos.y) * rpPtr->puMeshRect.h + rpPtr->puMeshRect.y);
                    p.x -= rpPtr->puMeshRect.w * 0.5f;
                    p.y -= rpPtr->puMeshRect.h * 0.5f;
                    p *= vDisplayQuality;

                    ImGuizmo::DrawNormal(p.x, p.y, n.x, n.y, n.z, glm::value_ptr(m), glm::value_ptr(vCamera->uProj), 10.0f, 5.0f, puNormalLength);

                    ImGui::BeginTooltip();
                    ImGui::Text("Normal : x:%.3f y:%.3f z:%.3f", n.x, n.y, n.z);
                    ImGui::EndTooltip();
                } else {
                    pos.x = (pos.x - rpPtr->puMeshRect.x) / rpPtr->puMeshRect.w;
                    pos.y = 1.0f - (pos.y - rpPtr->puMeshRect.y) / rpPtr->puMeshRect.h;

                    const auto n = puValues[puAttachment_Normal].xy();
                    ct::fvec4 line;
                    line.x = pos.x;
                    line.y = pos.y;
                    line.z = n.x * puNormalLength + pos.x;
                    line.w = n.y * puNormalLength + pos.y;
                    DrawLine(&line, &rpPtr->puMeshRect, vDisplayQuality);
                    DrawPoint(&pos, &rpPtr->puMeshRect, vDisplayQuality);

                    ImGui::BeginTooltip();
                    ImGui::Text("Normal : x:%.3f y:%.3f", n.x, n.y);
                    ImGui::EndTooltip();
                }
            }
        }
    }
}

void PixelDebugNormal::DrawPane(RenderPackWeak vRenderPack, float /*vDisplayQuality*/, CameraSystem* vCamera) {
    if (!puActivated)
        return;

    auto rpPtr = vRenderPack.lock();
    if (rpPtr) {
        if (ImGui::CollapsingHeader("Nomals Debug")) {
            if (vCamera) {
                ImGui::CheckBoxBoolDefault("Calc 3D Normals", &puUse3D, false);
            }

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Buffer Ids :");
            char buffer[10];

            ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.0f, 1.0f), "Normal :");
            ImGui::SameLine();
            for (auto i = 0; i <= rpPtr->puZeroBasedMaxSliceBufferId; i++) {
                if (i > 0)
                    ImGui::SameLine();
                snprintf(buffer, 9, "%i", i);
                if (ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), buffer, buffer, puAttachment_Normal == i)) {
                    puAttachment_Normal = i;
                }
            }
            // ImGui::SliderIntDefault(-1, "DF", &puAttachment_DF, -1, vRenderPack->puZeroBasedMaxSliceBufferId, -1);
            // ImGui::SliderIntDefault(150, "Normal", &puAttachment_Normal, -1, vRenderPack->puZeroBasedMaxSliceBufferId, -1);

            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Params :");
            ImGui::SliderFloat(-1, "Normal Length", &puNormalLength, 0.0f, 1.0f, 0.2f);
        }
    }
}

void PixelDebugNormal::Capture(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* /*vCamera*/) {
    if (!puActivated)
        return;

    auto rpPtr = vRenderPack.lock();
    if (rpPtr && rpPtr->GetPipe()) {
        const auto mi = MainBackend::Instance()->puMouseInterface;
        const ct::fvec2 mousePos(mi.px, mi.py);
        auto pos = mousePos / vDisplayQuality;
        if (rpPtr->puMeshRect.IsContainPoint(pos)) {
            pos.x = (pos.x - rpPtr->puMeshRect.x) / rpPtr->puMeshRect.w;
            pos.y = 1.0f - (pos.y - rpPtr->puMeshRect.y) / rpPtr->puMeshRect.h;

            if (rpPtr->GetFBOValuesAtNormalizedPos(pos, puValues, 8, false)) {
            }
        }
    }
}

void PixelDebugNormal::Resize(RenderPackWeak /*vRenderPack*/, float /*vDisplayQualit*/, CameraSystem* /*vCameray*/) {
    /*auto rpPtr = vRenderPack.lock();
    if (rpPtr && rpPtr->GetPipe())
    {

    }*/
}

bool PixelDebugNormal::NeedRefresh() {
    return puNeedRefresh;
}

void PixelDebugNormal::DrawPoint(ct::fvec2* vPoint, ct::frect* vRect, float vDisplayQuality) {
    auto& g = *GImGui;
    if (g.CurrentViewport) {
        auto drawList = ImGui::GetWindowDrawList();

        const auto p = g.CurrentViewport->Pos + ImVec2(vPoint->x * vRect->w + vRect->x, (1.0f - vPoint->y) * vRect->h + vRect->y) * vDisplayQuality;

        drawList->AddCircleFilled(p, 5.0f, ImColor(100, 100, 0));
        drawList->AddCircle(p, 5.0f, ImColor(255, 255, 255), 12, 2.0f);
    }
}

void PixelDebugNormal::DrawLine(ct::fvec4* vLine, ct::frect* vRect, float vDisplayQuality) {
    auto& g = *GImGui;
    if (g.CurrentViewport) {
        auto drawList = ImGui::GetWindowDrawList();

        const auto p0 = g.CurrentViewport->Pos + ImVec2(vLine->x * vRect->w + vRect->x, (1.0f - vLine->y) * vRect->h + vRect->y) * vDisplayQuality;
        const auto p1 = g.CurrentViewport->Pos + ImVec2(vLine->z * vRect->w + vRect->x, (1.0f - vLine->w) * vRect->h + vRect->y) * vDisplayQuality;

        drawList->AddLine(p0, p1, ImColor(200, 200, 100), 2.0f);
    }
}

/////////////////////////////////////////////////////////////
/////// PRIVATE /////////////////////////////////////////////
/////////////////////////////////////////////////////////////

std::string PixelDebugNormal::getXml(const std::string& vOffset, const std::string& vUserDatas) {
    UNUSED(vUserDatas);

    std::string str;

    str += vOffset + "<active>" + ct::toStr(puActivated ? "true" : "false") + "</active>\n";
    str += vOffset + "<attachment_DF>" + ct::toStr(puAttachment_DF) + "</attachment_DF>\n";
    str += vOffset + "<attachment_Normal>" + ct::toStr(puAttachment_Normal) + "</attachment_Normal>\n";
    str += vOffset + "<Normal_Length>" + ct::toStr(puNormalLength) + "</Normal_Length>\n";
    str += vOffset + "<Normal_3D>" + ct::toStr(puUse3D ? "true" : "false") + "</Normal_3D>\n";

    return str;
}

bool PixelDebugNormal::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) {
    UNUSED(vUserDatas);

    // The value of this child identifies the name of this element
    std::string strName = "";
    std::string strValue = "";
    std::string strParentName = "";

    strName = vElem->Value();
    if (vElem->GetText())
        strValue = vElem->GetText();
    if (vParent != nullptr)
        strParentName = vParent->Value();

    if (strName == "active")
        puActivated = ct::ivariant(strValue).GetB();
    if (strName == "attachment_DF")
        puAttachment_DF = ct::ivariant(strValue).GetI();
    if (strName == "attachment_Normal")
        puAttachment_Normal = ct::ivariant(strValue).GetI();
    if (strName == "Normal_3D")
        puUse3D = ct::ivariant(strValue).GetB();
    if (strName == "Normal_Length")
        puNormalLength = ct::fvariant(strValue).GetF();

    return true;
}