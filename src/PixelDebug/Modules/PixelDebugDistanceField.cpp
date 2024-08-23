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


#include "PixelDebugDistanceField.h"
#include <ImGuiPack.h>
#include <Backends/MainBackend.h>

#include <SoGLSL/src/Gui/CustomGuiWidgets.h>
#include <SoGLSL/src/Renderer/RenderPack.h>

#include <Gui/MainFrame.h>

PixelDebugDistanceField::PixelDebugDistanceField()
{
	pulastParsedAttachmentId = -1;
	puNeedRefresh = false;

	puAttachment_DF = -1;
	puAttachment_Normal = -1;

	puScale = 1.0f;

	puColor = ct::fvec3(1, 1, 1);

	puActivated = false;
}

PixelDebugDistanceField::~PixelDebugDistanceField()
{

}

void PixelDebugDistanceField::Init(RenderPackWeak  /*vRenderPack*/)
{
	puActivated = false;
}

void PixelDebugDistanceField::DrawMenu()
{

}

void PixelDebugDistanceField::DrawTooltips(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* /*vCamera*/)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe() && MainFrame::sCentralWindowHovered)
	{
		const auto mi = MainBackend::Instance()->puMouseInterface;
		const ct::fvec2 mousePos(mi.px, mi.py);
		auto pos = mousePos / vDisplayQuality;
		if (rpPtr->puMeshRect.IsContainPoint(pos))
		{
			puNeedRefresh = true;

			if (puAttachment_DF > -1)
			{
				pos.x = (pos.x - rpPtr->puMeshRect.x) / rpPtr->puMeshRect.w;
				pos.y = 1.0f - (pos.y - rpPtr->puMeshRect.y) / rpPtr->puMeshRect.h;

				auto dist = 0.0f;

				if (puAttachment_Normal > -1)
				{
					ct::fvec4 n[8];
					ct::fvec4 d[8];
					dist = 0.0f;
					puFinalPos = pos;
					for (auto i = 0; i < COUNT_POINTS; i++)
					{
						rpPtr->GetFBOValuesAtNormalizedPos(puFinalPos, n, 8, false);
						rpPtr->GetFBOValuesAtNormalizedPos(puFinalPos, d, 8, false);
						dist = d[puAttachment_DF].x;
						puFinalPos = pos + n[puAttachment_Normal].xy() * dist / puScale;
						puPoints[i] = puFinalPos;
						DrawPoint(&puPoints[i], &rpPtr->puMeshRect, vDisplayQuality);
					}
					
					ct::fvec4 line(pos.x, pos.y, puFinalPos.x, puFinalPos.y);
					DrawLine(&line, &rpPtr->puMeshRect, vDisplayQuality);
					DrawPoint(&puFinalPos, &rpPtr->puMeshRect, vDisplayQuality);
					DrawCircle(&pos, dist * puScale, &rpPtr->puMeshRect, vDisplayQuality);
				}
				else
				{
					dist = puValues[puAttachment_DF].x;
					DrawCircle(&pos, dist * puScale, &rpPtr->puMeshRect, vDisplayQuality);
				}

				ImGui::BeginTooltip();
				ImGui::Text("Distance to DF : %.3f", dist);
				ImGui::EndTooltip();
			}
		}
	}
}

void PixelDebugDistanceField::DrawPane(RenderPackWeak vRenderPack, float /*vDisplayQuality*/, CameraSystem* /*vCamera*/)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr)
	{
		if (ImGui::CollapsingHeader("Distance Field Debug"))
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Buffer Ids :");
			char buffer[10];

			ImGui::TextColored(ImVec4(1, 0, 1, 1), "Df :"); ImGui::SameLine();
			for (auto i = -1; i <= rpPtr->puZeroBasedMaxSliceBufferId; i++)
			{
				if (i > -1)
					ImGui::SameLine();
				snprintf(buffer, 9, "%i", i);
				if (ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), buffer, buffer, puAttachment_DF == i))
				{
					puAttachment_DF = i;
				}
			}

			if (puAttachment_DF > -1)
			{
				ImGui::TextColored(ImVec4(1, 0, 1, 1), "Normal :"); ImGui::SameLine();
				for (auto i = -1; i <= rpPtr->puZeroBasedMaxSliceBufferId; i++)
				{
					if (i > -1)
						ImGui::SameLine();
					snprintf(buffer, 9, "%i", i);
					if (ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), buffer, buffer, puAttachment_Normal == i))
					{
						puAttachment_Normal = i;
					}
				}
			}
			
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Scale :"); ImGui::SameLine();
			ImGui::SliderFloat(150, "##Scale", &puScale, 0.0f, 100.0f, 1.0f);

			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Color :"); ImGui::SameLine();
			ImGui::ColorEdit3("##colorValue", &puColor.x, ImGuiColorEditFlags_Uint8);
		}
	}
}

void PixelDebugDistanceField::Capture(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* /*vCamera*/)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		const auto mi = MainBackend::Instance()->puMouseInterface;
		const ct::fvec2 mousePos(mi.px, mi.py);
		auto pos = mousePos / vDisplayQuality;
		if (rpPtr->puMeshRect.IsContainPoint(pos))
		{
			pos.x = (pos.x - rpPtr->puMeshRect.x) / rpPtr->puMeshRect.w;
			pos.y = 1.0f - (pos.y - rpPtr->puMeshRect.y) / rpPtr->puMeshRect.h;
			
			if (rpPtr->GetFBOValuesAtNormalizedPos(pos, puValues, 8, false))
			{
				
			}
		}
	}
}

void PixelDebugDistanceField::Resize(RenderPackWeak /*vRenderPack*/, float /*vDisplayQuality*/, CameraSystem* /*vCamera*/)
{
	/*auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		
	}*/
}

bool PixelDebugDistanceField::NeedRefresh()
{
	return puNeedRefresh;
}

void PixelDebugDistanceField::DrawPoint(ct::fvec2 *vPoint, ct::frect *vRect, float vDisplayQuality)
{
	auto& g = *GImGui;
	if (g.CurrentViewport)
	{
		auto drawList = ImGui::GetWindowDrawList();

		const auto p = g.CurrentViewport->Pos + ImVec2(vPoint->x * vRect->w + vRect->x, (1.0f - vPoint->y) * vRect->h + vRect->y) * vDisplayQuality;

		drawList->AddCircleFilled(p, 5.0f, ImColor(100, 100, 0));
		drawList->AddCircle(p, 5.0f, ImColor(255, 255, 255), 12, 2.0f);
	}
}

void PixelDebugDistanceField::DrawCircle(ct::fvec2 *vPoint, float vRadius, ct::frect *vRect, float vDisplayQuality)
{
	auto& g = *GImGui;
	if (g.CurrentViewport)
	{
		auto drawList = ImGui::GetWindowDrawList();

		const auto p = g.CurrentViewport->Pos + ImVec2(vPoint->x * vRect->w + vRect->x, (1.0f - vPoint->y) * vRect->h + vRect->y) * vDisplayQuality;
		const auto radius = ct::abs(vRadius / vDisplayQuality);

		auto countSegment = 12;
		const auto perimeter = 2.0f * _pi * radius;
		const auto space = 5.0f;
		countSegment = (int)(perimeter / space);
		drawList->AddCircle(p, 5.0f, ImGui::GetColorU32(ImVec4(puColor.x, puColor.y, puColor.z, 1.0f)), 12, 2.0f);
		drawList->AddCircle(p, radius, ImGui::GetColorU32(ImVec4(puColor.x, puColor.y, puColor.z, 1.0f)), countSegment, 2.0f);
	}
}

void PixelDebugDistanceField::DrawLine(ct::fvec4 *vLine, ct::frect *vRect, float vDisplayQuality)
{
	auto& g = *GImGui;
	if (g.CurrentViewport)
	{
		auto drawList = ImGui::GetWindowDrawList();

		const auto p0 = g.CurrentViewport->Pos + ImVec2(vLine->x * vRect->w + vRect->x, (1.0f - vLine->y) * vRect->h + vRect->y) * vDisplayQuality;
		const auto p1 = g.CurrentViewport->Pos + ImVec2(vLine->z * vRect->w + vRect->x, (1.0f - vLine->w) * vRect->h + vRect->y) * vDisplayQuality;

		drawList->AddLine(p0, p1, ImColor(200, 200, 100), 2.0f);
	}
}

/////////////////////////////////////////////////////////////
/////// PRIVATE /////////////////////////////////////////////
/////////////////////////////////////////////////////////////

std::string PixelDebugDistanceField::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + "<active>" + ct::toStr(puActivated ? "true" : "false") + "</active>\n";
	str += vOffset + "<attachment_DF>" + ct::toStr(puAttachment_DF) + "</attachment_DF>\n";
	str += vOffset + "<scale>" + ct::toStr(puScale) + "</scale>\n";
	str += vOffset + "<color>" + ct::toStr(puColor.string()) + "</color>\n";

	return str;
}

bool PixelDebugDistanceField::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

	if (strName == "active")
		puActivated = ct::ivariant(strValue).GetB();
	if (strName == "attachment_DF")
		puAttachment_DF = ct::ivariant(strValue).GetI();
	if (strName == "scale")
		puScale = ct::ivariant(strValue).GetF();
	if (strName == "color")
		puColor = ct::fvariant(strValue).GetV3();

	return true;
}