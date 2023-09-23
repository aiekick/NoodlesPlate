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

// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include "PixelDebug.h"
#include <ImGuiPack.h>

#include <SoGLSL/src/Gui/CustomGuiWidgets.h>
#include <SoGLSL/src/Renderer/RenderPack.h>
#include <SoGLSL/src/Systems/CameraSystem.h>

PixelDebug::PixelDebug()
{
	puMouseDebugPoint.SetMousePointType(true);
	puMeasureFrequencie = 30;

	Init();
}

PixelDebug::~PixelDebug()
{

}

void PixelDebug::Init(RenderPackWeak vRenderPack)
{
	puMouseDebugPoint.Init(vRenderPack);
	puLineDebugPoint.Init(vRenderPack);
	puNormalDebugPoint.Init(vRenderPack);
	puGizmoDebugPoint.Init(vRenderPack);
}

bool PixelDebug::IsActive()
{
	return 
		puMouseDebugPoint.IsActive()
		|| puLineDebugPoint.IsActive()
		|| puNormalDebugPoint.IsActive()
		|| puGizmoDebugPoint.IsActive();
}

void PixelDebug::DrawMenu()
{
	puMouseDebugPoint.DrawMenu();
	
	ImGui::Separator();

	puLineDebugPoint.DrawMenu();
	
	ImGui::Separator();
	
	puNormalDebugPoint.DrawMenu();
	
	ImGui::Separator();

	puGizmoDebugPoint.DrawMenu();
}

void PixelDebug::DrawTooltips(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem *vCamera)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr)
	{
		puMouseDebugPoint.DrawTooltips(vRenderPack, vDisplayQuality);
		puLineDebugPoint.DrawTooltips(vRenderPack, vDisplayQuality);
		puNormalDebugPoint.DrawTooltips(vRenderPack, vDisplayQuality, vCamera);
		puGizmoDebugPoint.DrawTooltips(vRenderPack, vDisplayQuality, vCamera);
	}
}

void PixelDebug::DrawPane(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem *vCamera)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetShaderKey())
	{
		if (IsActive())
		{
			if (ImGui::CollapsingHeader("Pixels Debugging"))
			{
				ImGui::TextColored(ImVec4(1,1,0,1), "Measure Frequencie (Count per sec or Max FPS)");
				if (ImGui::SliderIntDefault(150, "N/s", &puMeasureFrequencie, 1, 120, 30))
				{
					puMeasureFrequencie = ct::clamp(puMeasureFrequencie, 1, 120);
				}
				
				ImGui::Indent();

				puMouseDebugPoint.DrawPane(vRenderPack, vDisplayQuality);
				puLineDebugPoint.DrawPane(vRenderPack, vDisplayQuality);
				puNormalDebugPoint.DrawPane(vRenderPack, vDisplayQuality, vCamera);
				puGizmoDebugPoint.DrawPane(vRenderPack, vDisplayQuality, vCamera);

				ImGui::Unindent();
			}
		}
	}
}

void PixelDebug::Capture(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem *vCamera)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		// 1000 => 1
		// 100 => 10
		// 10 => 100
		// 1000 / freq
		if (puActionTime.IsTimeToAct(1000 / puMeasureFrequencie, true)) // 1000 => 1s / 100 => 0.1s (10 fois par secs)
		{
			puMouseDebugPoint.Capture(vRenderPack, vDisplayQuality);
			puLineDebugPoint.Capture(vRenderPack, vDisplayQuality);
			puNormalDebugPoint.Capture(vRenderPack, vDisplayQuality, vCamera);
			puGizmoDebugPoint.Capture(vRenderPack, vDisplayQuality, vCamera);
		}
	}
}

void PixelDebug::Resize(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem *vCamera)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		puLineDebugPoint.Resize(vRenderPack, vDisplayQuality);
		puMouseDebugPoint.Resize(vRenderPack, vDisplayQuality);
		puNormalDebugPoint.Resize(vRenderPack, vDisplayQuality, vCamera);
		puGizmoDebugPoint.Resize(vRenderPack, vDisplayQuality, vCamera);
	}
}

bool PixelDebug::NeedRefresh()
{
	return
		puLineDebugPoint.NeedRefresh()
		|| puMouseDebugPoint.NeedRefresh()
		|| puNormalDebugPoint.NeedRefresh()
		|| puGizmoDebugPoint.NeedRefresh();
}

std::string PixelDebug::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	auto offset = vOffset;

	str += vOffset + "<pixeldebug>\n";
	
	offset += vOffset;

	str += offset + "<debug_measurefreq>" + ct::toStr(puMeasureFrequencie) + "</debug_measurefreq>\n";
	
	str += offset + "<mouse>\n";
	str += puMouseDebugPoint.getXml(offset + "\t", vUserDatas);
	str += offset + "</mouse>\n";
	
	str += offset + "<line>\n";
	str += puLineDebugPoint.getXml(offset + "\t", vUserDatas);
	str += offset + "</line>\n";

	str += offset + "<normal>\n";
	str += puNormalDebugPoint.getXml(offset + "\t", vUserDatas);
	str += offset + "</normal>\n";

	str += offset + "<gizmo>\n";
	str += puGizmoDebugPoint.getXml(offset + "\t", vUserDatas);
	str += offset + "</gizmo>\n";

	str += vOffset + "</pixeldebug>\n";

	return str;
}

bool PixelDebug::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
{
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

	if (strName == "debug_measurefreq")
		puMeasureFrequencie = ct::ivariant(strValue).GetI();

	if (strName == "mouse")
	{
		for (auto child = vElem->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			puMouseDebugPoint.RecursParsingConfig(child->ToElement(), vElem); // ca me cree un bug de stackoverflow.. pendant la capture des data de la souris
		}
	}

	if (strName == "line")
	{
		for (auto child = vElem->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			puLineDebugPoint.RecursParsingConfig(child->ToElement(), vElem); // ca me cree un bug de stackoverflow.. pendant la capture des data de la souris
		}
	}

	if (strName == "normal")
	{
		for (auto child = vElem->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			puNormalDebugPoint.RecursParsingConfig(child->ToElement(), vElem); // ca me cree un bug de stackoverflow.. pendant la capture des data de la souris
		}
	}

	if (strName == "gizmo")
	{
		for (auto child = vElem->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			puGizmoDebugPoint.RecursParsingConfig(child->ToElement(), vElem); // ca me cree un bug de stackoverflow.. pendant la capture des data de la souris
		}
	}

	return true;
}