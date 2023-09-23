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


#include "PixelDebugPoint.h"
#include <ImGuiPack.h>

#include <SoGLSL/src/Gui/CustomGuiWidgets.h>
#include <SoGLSL/src/Renderer/RenderPack.h>
#include <SoGLSL/src/Systems/CameraSystem.h>


#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>

#define DEBUG_POINT_HISTO_COUNT 200

PixelDebugPoint::PixelDebugPoint()
{
	pulastParsedAttachmentId = -1;
	puNeedRefresh = false;

	puActivated = false;

	for (auto i = 0; i < 8; i++)
	{
		puCountPointToShowOnGraph[i] = DEBUG_POINT_HISTO_COUNT - 1;
		puValuesHisto[i] = new ct::fvec4[DEBUG_POINT_HISTO_COUNT];
		puGraphLimits[i] = ct::fvec2(0.0f, 1.0f);
		puUseAttachment[i] = true;
		puImagePixelDisplay[i] = 5;
		puShowImage[i] = false;

		puLimitType[i] = PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_AUTO;
		puPixelDebugMeasureLimitTypeIdx[i] = (int)puLimitType[i];

		for (auto j = 0; j < 4; j++)
		{
			auto res = true;
			if (j == 3) res = false;
			puShowLine[i][j] = res;
		}
	}

	puIsMousePoint = false;

	puCanCatchPixelDebugIfMousePosChange = true;
	puCanCatchPixelDebugIfValueChange = true;
}

PixelDebugPoint::~PixelDebugPoint()
{
	for (auto i = 0; i < 8; i++)
	{
		SAFE_DELETE_ARRAY(puValuesHisto[i]);
	}
}

void PixelDebugPoint::Init(RenderPackWeak  /*vRenderPack*/)
{
	puActivated = false;
}

void PixelDebugPoint::DrawMenu()
{
	ImGui::Checkbox("Under Mouse", &puActivated);

	if (puActivated)
	{
		ImGui::Indent();
		ImGui::Checkbox("Capture if Mouse Pos change", &puCanCatchPixelDebugIfMousePosChange);
		ImGui::Checkbox("Capture if Value change", &puCanCatchPixelDebugIfValueChange);
		ImGui::Unindent();
	}
}

void PixelDebugPoint::SetMousePointType(bool vIsMousePoint)
{
	puIsMousePoint = vIsMousePoint;
}

bool PixelDebugPoint::IsMousePointType()
{
	return puIsMousePoint;
}

void PixelDebugPoint::DrawTooltips(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* /*vCamera*/)
{
	if (!puActivated) return;

	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe() && MainFrame::sCentralWindowHovered)
	{
		auto& g = *GImGui;
		if (g.CurrentViewport)
		{
			const auto mi = MainBackend::Instance()->puMouseInterface;
			const ct::fvec2 mousePos(mi.px, mi.py);
			const auto pos = mousePos / vDisplayQuality;
			if (rpPtr->puMeshRect.IsContainPoint(pos))
			{
				puNeedRefresh = true;

				ImGui::GetCurrentWindow()->DrawList->AddLine(
					ImVec2(g.CurrentViewport->Pos.x + rpPtr->puMeshRect.left * vDisplayQuality, g.CurrentViewport->Pos.y + mousePos.y),
					ImVec2(g.CurrentViewport->Pos.x + rpPtr->puMeshRect.right * vDisplayQuality, g.CurrentViewport->Pos.y + mousePos.y),
					ImGui::GetColorU32(ImVec4(1, 1, 0, 1)));
				ImGui::GetCurrentWindow()->DrawList->AddLine(
					ImVec2(g.CurrentViewport->Pos.x + mousePos.x, g.CurrentViewport->Pos.y + rpPtr->puMeshRect.top * vDisplayQuality),
					ImVec2(g.CurrentViewport->Pos.x + mousePos.x, g.CurrentViewport->Pos.y + rpPtr->puMeshRect.bottom * vDisplayQuality),
					ImGui::GetColorU32(ImVec4(1, 1, 0, 1)));

				ImGui::BeginTooltip();
				ImGui::Text("Pos : x:%.3f y:%.3f", puLastNormalizedTexturePos.x, puLastNormalizedTexturePos.y);
				for (auto i = 0; i <= rpPtr->puZeroBasedMaxSliceBufferId; i++)
				{
					if (puUseAttachment[i])
					{
						ImGui::Text("Att %i (%0.3f, %0.3f, %0.3f, %0.3f)", i,
							puValues[i].x, puValues[i].y, puValues[i].z, puValues[i].w);
					}
				}
				ImGui::EndTooltip();
			}
		}
	}
}

void PixelDebugPoint::DrawPane(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* /*vCamera*/)
{
	if (!puActivated) return;

	auto rpPtr = vRenderPack.lock();
	if (rpPtr)
	{
		auto res = false;
		
		if (puIsMousePoint)
			res = ImGui::CollapsingHeader("Under Mouse");
		else
			res = ImGui::CollapsingHeader("Fixed Point");

		if (res)
		{
			for (auto i = 0; i <= rpPtr->puZeroBasedMaxSliceBufferId; i++)
			{
				DrawGraph(i, vRenderPack, vDisplayQuality);
			}
		}
	}
}

void PixelDebugPoint::Capture(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* /*vCamera*/)
{
	if (!puActivated) return;

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

			memcpy(puLastValues, puValues, sizeof(ct::fvec4) * 8);
			
			if (rpPtr->GetFBOValuesAtNormalizedPos(pos, puValues, 8, puCanCatchPixelDebugIfMousePosChange))
			{
				puLastNormalizedTexturePos.x = pos.x;
				puLastNormalizedTexturePos.y = pos.y;

				Historize(vRenderPack, puCanCatchPixelDebugIfValueChange);
			}
		}
	}
}

void PixelDebugPoint::Resize(RenderPackWeak /*vRenderPack*/, float /*vDisplayQuality*/, CameraSystem* /*vCamera*/)
{
	/*auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		
	}*/
}

bool PixelDebugPoint::NeedRefresh()
{
	return puNeedRefresh;
}

/////////////////////////////////////////////////////////////
/////// PRIVATE /////////////////////////////////////////////
/////////////////////////////////////////////////////////////

// prend la derniere valeur de puPixelDebugValues
// la stock dans le tableau d'historique en gerant les bornes du tableau
void PixelDebugPoint::Historize(RenderPackWeak vRenderPack, bool vCanHistorizeIfValueChange)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr)
	{
		const auto s = sizeof(ct::fvec4);
		const size_t zbs = DEBUG_POINT_HISTO_COUNT - 1; // zero based size
		for (auto i = 0; i <= rpPtr->puZeroBasedMaxSliceBufferId; i++)
		{
			if (puUseAttachment[i])
			{
				if (!vCanHistorizeIfValueChange || (vCanHistorizeIfValueChange && puValues[i] != puLastValues[i]))
				{
					if (puLimitType[i] == PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_MIN_MAX_HISTO)
					{
						if (puShowLine[i][0])
						{
							puGraphLimits[i].x = ct::mini(puGraphLimits[i].x, puValues[i].x);
							puGraphLimits[i].y = ct::maxi(puGraphLimits[i].y, puValues[i].x);
						}
						if (puShowLine[i][1])
						{
							puGraphLimits[i].x = ct::mini(puGraphLimits[i].x, puValues[i].y);
							puGraphLimits[i].y = ct::maxi(puGraphLimits[i].y, puValues[i].y);
						}
						if (puShowLine[i][2])
						{
							puGraphLimits[i].x = ct::mini(puGraphLimits[i].x, puValues[i].z);
							puGraphLimits[i].y = ct::maxi(puGraphLimits[i].y, puValues[i].z);
						}
						if (puShowLine[i][3])
						{
							puGraphLimits[i].x = ct::mini(puGraphLimits[i].x, puValues[i].w);
							puGraphLimits[i].y = ct::maxi(puGraphLimits[i].y, puValues[i].w);
						}
					}
					
					// decale les valeurs de puPixelDebugValuesHisto de 1
					memmove(puValuesHisto[i], puValuesHisto[i] + 1, s * (zbs - 1));
					// et met puPixelDebugValues dans puPixelDebugValuesHisto a l'index 0
					puValuesHisto[i][zbs - 1] = puValues[i];
				}
			}
		}
	}
}

void PixelDebugPoint::DrawGraph(size_t vAttachment, RenderPackWeak vRenderPack, float /*vDisplayQuality*/)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr)
	{
		if (vAttachment <= rpPtr->puZeroBasedMaxSliceBufferId)
		{
			char buffer[50] = "\0";
			snprintf(buffer, 50, "Attachment %u", (uint32_t)vAttachment);

			ImGui::BeginFramedGroup(buffer);

			//ImGui::Text("Attachment %i :", vAttachment);

			ImGui::CheckBoxBoolDefault("Use ?", &puUseAttachment[vAttachment], true);

			if (puUseAttachment[vAttachment])
			{
				if (ImGui::ContrastedCombo(150, "Limits", &puPixelDebugMeasureLimitTypeIdx[vAttachment], "AUTO\0CUSTOM\0MIN MAX HISTO\0\0", -1))
				{
					puLimitType[vAttachment] = (PixelDebugMeasureLimitType)puPixelDebugMeasureLimitTypeIdx[vAttachment];

					if (puLimitType[vAttachment] == PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_MIN_MAX_HISTO)
					{
						puGraphLimits[vAttachment].x = 0;
						puGraphLimits[vAttachment].y = 0;
					}
				}

				if (puLimitType[vAttachment] == PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_AUTO)
				{
					puGraphLimits[vAttachment].x = FLT_MAX;
					puGraphLimits[vAttachment].y = FLT_MAX;
				}
				else if (puLimitType[vAttachment] == PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_CUSTOM)
				{
					ImGui::InputFloatDefault(70.0f, "##MinY", &puGraphLimits[vAttachment].x, 0.0f); ImGui::SameLine();
					ImGui::InputFloatDefault(70.0f, "##MaxY", &puGraphLimits[vAttachment].y, 1.0f);
				}

				ImGui::BeginGroup();
				ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), "R##SHOW_RED_CHANNEL", "Show Red Channel", &puShowLine[vAttachment][0]);
				ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), "G##SHOW_GREEN_CHANNEL", "Show Green Channel", &puShowLine[vAttachment][1]);
				ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), "B##SHOW_BLUE_CHANNEL", "Show Blue Channel", &puShowLine[vAttachment][2]);
				ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), "A##SHOW_ALPHA_CHANNEL", "Show Alpha Channel", &puShowLine[vAttachment][3]);
				ImGui::EndGroup();
				ImGui::SameLine();
				ImGui::PlotFVec4Histo("##SHOW_CHANNEL", puValuesHisto[vAttachment], puCountPointToShowOnGraph[vAttachment],
					puShowLine[vAttachment], ImVec2(200, 92),
					puGraphLimits[vAttachment].x, puGraphLimits[vAttachment].y);
				if (ImGui::SliderIntDefault(200, "Histo", &puCountPointToShowOnGraph[vAttachment], 5, DEBUG_POINT_HISTO_COUNT - 1, DEBUG_POINT_HISTO_COUNT - 1))
				{
					puCountPointToShowOnGraph[vAttachment] = ct::clamp(puCountPointToShowOnGraph[vAttachment], 5, DEBUG_POINT_HISTO_COUNT - 1);
				}

				ImGui::CheckBoxBoolDefault("Show Pixel Preview", &puShowImage[vAttachment], true);
				if (puShowImage[vAttachment])
				{
					ImGui::SliderIntDefault(150, "Pixels", &puImagePixelDisplay[vAttachment], 1, 20, 5);
					const auto center = ImVec2(puLastNormalizedTexturePos.x, puLastNormalizedTexturePos.y);
					const auto point = ct::toImVec2(ct::fvec2(1.0f / (float)rpPtr->GetSize().x, -1.0f / (float)rpPtr->GetSize().x));
					const auto radiusInPixel = ImVec2((float)puImagePixelDisplay[vAttachment], (float)puImagePixelDisplay[vAttachment]);
					ImGui::ImageZoomPoint((ImTextureID)(size_t)rpPtr->GetTextureId((int)vAttachment), 200, center, point, radiusInPixel);
				}
			}
			
			ImGui::EndFramedGroup();
		}
	}
}

std::string PixelDebugPoint::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + "<active>" + ct::toStr(puActivated ? "true" : "false") + "</active>\n";
	str += vOffset + "<ifmoving>" + ct::toStr(puCanCatchPixelDebugIfMousePosChange ? "true" : "false") + "</ifmoving>\n";
	str += vOffset + "<ifvaluechange>" + ct::toStr(puCanCatchPixelDebugIfValueChange ? "true" : "false") + "</ifvaluechange>\n";

	for (auto i = 0; i < 8; i++)
	{
		str += vOffset + "<attachments value=\"" + ct::toStr(i) + "\">\n";
		str += vOffset + "\t<use>" + ct::toStr(puUseAttachment[i] ? "true" : "false") + "</use>\n";
		str += vOffset + "\t<channels value=\"";
		for (auto c = 0; c < 4; c++)
		{
			if (c > 0)
				str += ";";
			str += ct::toStr(puShowLine[i][c] ? "1" : "0");
		}
		str += "\"/>\n";;
		str += vOffset + "\t<limits value=\"" + puGraphLimits[i].string() + "\" type=\"" + ct::toStr(puLimitType[i]) + "\"/>\n";
		str += vOffset + "\t<histo value=\"" + ct::toStr(puCountPointToShowOnGraph[i]) + "\"/>\n";
		str += vOffset + "</attachments>\n";
	}
	
	return str;
}

bool PixelDebugPoint::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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
	if (strName == "ifmoving")
		puCanCatchPixelDebugIfMousePosChange = ct::ivariant(strValue).GetB();
	if (strName == "ifvaluechange")
		puCanCatchPixelDebugIfValueChange = ct::ivariant(strValue).GetB();

	if (strName == "attachments")
	{
		auto att = vElem->FirstAttribute();
		if (att)
		{
			strName = att->Name();
			if (strName == "value")
			{
				strValue = att->Value();
				pulastParsedAttachmentId = ct::ivariant(strValue).GetI();
			}
		}
	}

	if (strName == "use" && strParentName == "attachments")
	{
		if (pulastParsedAttachmentId >= 0 && pulastParsedAttachmentId < 8)
		{
			puUseAttachment[pulastParsedAttachmentId] = ct::ivariant(strValue).GetB();
		}
	}

	if (strName == "channels" && strParentName == "attachments")
	{
		auto att = vElem->FirstAttribute();
		if (att)
		{
			strName = att->Name();
			if (strName == "value")
			{
				strValue = att->Value();
				if (pulastParsedAttachmentId >= 0 && pulastParsedAttachmentId < 8)
				{
					ct::ivec4 v(strValue, ';', 4);
					puShowLine[pulastParsedAttachmentId][0] = (bool)v.x;
					puShowLine[pulastParsedAttachmentId][1] = (bool)v.y;
					puShowLine[pulastParsedAttachmentId][2] = (bool)v.z;
					puShowLine[pulastParsedAttachmentId][3] = (bool)v.w;
				}
			}
		}
	}

	if (strName == "limits" && strParentName == "attachments")
	{
		for (auto attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next())
		{
			std::string attName = attr->Name();
			std::string attValue = attr->Value();

			if (attName == "value")
			{
				if (pulastParsedAttachmentId >= 0 && pulastParsedAttachmentId < 8)
				{
					puGraphLimits[pulastParsedAttachmentId] = ct::fvariant(attValue).GetV2();
				}
			}
			if (attName == "type")
			{
				if (pulastParsedAttachmentId >= 0 && pulastParsedAttachmentId < 8)
				{
					puPixelDebugMeasureLimitTypeIdx[pulastParsedAttachmentId] = ct::ivariant(attValue).GetI();
					puLimitType[pulastParsedAttachmentId] = (PixelDebugMeasureLimitType)puPixelDebugMeasureLimitTypeIdx[pulastParsedAttachmentId];

					if (puLimitType[pulastParsedAttachmentId] == PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_MIN_MAX_HISTO)
					{
						puGraphLimits[pulastParsedAttachmentId].x = 0;
						puGraphLimits[pulastParsedAttachmentId].y = 0;
					}
				}
			}
		}
	}

	if (strName == "histo" && strParentName == "attachments")
	{
		auto att = vElem->FirstAttribute();
		if (att)
		{
			strName = att->Name();
			if (strName == "value")
			{
				strValue = att->Value();
				if (pulastParsedAttachmentId >= 0 && pulastParsedAttachmentId < 8)
				{
					puCountPointToShowOnGraph[pulastParsedAttachmentId] = ct::ivariant(strValue).GetI();
				}
			}
		}
	}

	return true;
}