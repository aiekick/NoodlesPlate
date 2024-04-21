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

#include "PixelDebugHeader.h"

class RenderPack;
class PixelDebugPoint : public PixelDebugAbstract, public conf::ConfigAbstract
{
private:
	bool puActivated = false;
	bool puNeedRefresh = false;
	bool puIsMousePoint = false;
	bool puUseAttachment[8];
	bool puShowLine[8][4];
	int puImagePixelDisplay[8];
	bool puShowImage[8];
	ct::fvec2 puGraphLimits[8];
	ct::fvec4 puLastValues[8];
	ct::fvec4 puValues[8];
	ct::fvec4 *puValuesHisto[8];
	ct::fvec2 puLastNormalizedTexturePos;
	int puCountPointToShowOnGraph[8];
	PixelDebugMeasureLimitType puLimitType[8];
	int puPixelDebugMeasureLimitTypeIdx[8];
	bool puCanCatchPixelDebugIfMousePosChange = false;
	bool puCanCatchPixelDebugIfValueChange = false;

public:
	PixelDebugPoint();
	~PixelDebugPoint();
	
	void SetMousePointType(bool vIsMousePoint);
	bool IsMousePointType();

	void Init(RenderPackWeak vRenderPack = RenderPackWeak()) override;
	bool IsActive() override { return puActivated; }
	void DrawMenu() override;
	void DrawTooltips(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) override;
	void DrawPane(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) override;
	void Capture(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) override;
	void Resize(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) override;
	bool NeedRefresh() override;

private:
	void Historize(RenderPackWeak vRenderPack, bool vCanHistorizeIfValueChange = false);
	void DrawGraph(size_t vAttachment, RenderPackWeak vRenderPack, float vDisplayQuality);

public:
	int pulastParsedAttachmentId;
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;
};
