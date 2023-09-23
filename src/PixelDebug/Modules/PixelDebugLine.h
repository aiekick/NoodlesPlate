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

#pragma once

#include <ctools/cTools.h>
#include <ctools/ConfigAbstract.h>
#include <PixelDebug/Abtract/PixelDebugAbstract.h>

#include "PixelDebugHeader.h"

class RenderPack;
class PixelDebugLine : public PixelDebugAbstract, public conf::ConfigAbstract
{
private:
	bool puActivated = false;
	bool puNeedRefresh = false;
	int puCurrentAttachment = 0;

	bool puIsMousePoint = false;
	bool puShowLine[4];
	int puImagePixelDisplay = 0;
	bool puShowImage = false;
	ct::fvec4 *puValuesHisto;
	ct::fvec2 puLastNormalizedTexturePos;
	int puCountPointToShowOnGraph = 0;

	ct::fvec2 puFirstPos;
	ct::fvec2 puLastPos;
	int puPointToPlace = 0;

	ct::fAABB puBufferAABB;
	
	ct::fvec2 puIterStartPos;
	ct::fvec2 puIterDirection;
	
	float *puBuffer = nullptr;
	ct::ivec2 puCapturePos;
	ct::ivec2 puBufferSize;
	size_t puBufferLength = 0;
	size_t puRGBAOffset = 0;
	std::vector<ct::fvec3> puCapturesPoints;
	
	int puGraphHoveredIdx = 0;

	ct::fvec2 puGraphLimits;
	PixelDebugMeasureLimitType puLimitType = (PixelDebugMeasureLimitType)0;
	int puPixelDebugMeasureLimitTypeIdx = 0;

public:
	PixelDebugLine();
	~PixelDebugLine();
	
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
	void SaveBufferToFile(const std::string& vFile);

	void InitBufferVariables();
	void CreateBuffer(RenderPackWeak vRenderPack, ct::fvec2 vStartPos, ct::fvec2 vEndPos);
	void DestroyBuffer();
	void CaptureFromGPU(RenderPackWeak vRenderPack, int vAttachmentId);
	void PrepareDatas(RenderPackWeak vRenderPack, int vAttachmentId);

	void DrawGraph(size_t vAttachment, RenderPackWeak vRenderPack, float vDisplayQuality);

public:
	int pulastParsedAttachmentId;
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;

};
