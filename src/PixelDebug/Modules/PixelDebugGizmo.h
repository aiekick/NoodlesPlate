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
#include <SoGLSL/src/Uniforms/UniformVariant.h>
#include <PixelDebug/Abtract/PixelDebugAbstract.h>

class CameraSystem;
class RenderPack;
class UniformVariant;
class PixelDebugGizmo : public PixelDebugAbstract, public conf::ConfigAbstract
{
private:
	bool puActivated = false;
	int puAttachment_Gizmo = 0;
	ct::fvec4 puValues[8];
	std::vector<ct::fvec4> puMeasurePoints;

	bool puNeedRefresh = false;
	ct::fvec2 puTexel;
	bool puUseUnderMouse = false;
	UniformVariantPtr puGizmo = nullptr;
	std::string puGizmoName;

public:
	PixelDebugGizmo();
	~PixelDebugGizmo();
	
	void Init(RenderPackWeak vRenderPack) override;

	bool IsActive() override {
        return puActivated;
    }
    void DrawMenu() override;
    void DrawTooltips(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem *vCamera) override;
    void DrawPane(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem *vCamera) override;
    void Capture(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem *vCamera) override;
    void Resize(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem *vCamera) override;
    bool NeedRefresh() override;

private:
	void DrawPoint(ct::fvec2 *vPoint, ct::frect *vRect, float vDisplayQuality);
	void DrawLine(ct::fvec4 *vLine, ct::frect *vRect, float vDisplayQuality);

public:
	int pulastParsedAttachmentId;
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;

};
