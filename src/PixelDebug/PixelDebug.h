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

#pragma once

#include <ctools/cTools.h>
#include <ctools/ConfigAbstract.h>

#include <PixelDebug/Abtract/PixelDebugAbstract.h>
#include <PixelDebug/Modules/PixelDebugPoint.h>
#include <PixelDebug/Modules/PixelDebugLine.h>
#include <PixelDebug/Modules/PixelDebugNormal.h>
#include <PixelDebug/Modules/PixelDebugGizmo.h>

class CameraSystem;
class RenderPack;
class PixelDebug : public PixelDebugAbstract, public conf::ConfigAbstract {
public:
    // bool puActivate_PixelDebug_DF;

private:
    PixelDebugPoint puMouseDebugPoint;
    PixelDebugLine puLineDebugPoint;
    PixelDebugNormal puNormalDebugPoint;
    // PixelDebugDistanceField puDFDebugPoint;
    PixelDebugGizmo puGizmoDebugPoint;

private:
    ct::ActionTime puActionTime;
    int puMeasureFrequencie = 0;

public:
    PixelDebug();
    ~PixelDebug();

    void Init(RenderPackWeak vRenderPack = RenderPackWeak()) override;
    bool IsActive() override;
    void DrawMenu() override;
    void DrawTooltips(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) override;
    void DrawPane(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) override;
    void Capture(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) override;
    void Resize(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) override;
    bool NeedRefresh() override;

public:
    std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
    bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;
};
