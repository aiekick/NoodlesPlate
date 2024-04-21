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

#include <Headers/RenderPackHeaders.h>

class CameraSystem;
class PixelDebugAbstract
{
public:
	virtual void Init(RenderPackWeak vRenderPack = RenderPackWeak()) = 0;
	virtual bool IsActive() = 0;
	virtual void DrawMenu() = 0;
	virtual void DrawTooltips(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) = 0;
	virtual void DrawPane(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) = 0;
	virtual void Capture(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) = 0;
	virtual void Resize(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* vCamera = nullptr) = 0;
	virtual bool NeedRefresh() = 0;
};