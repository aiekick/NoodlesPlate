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


#include "PixelDebugLine.h"

#include <ctools/Logger.h>
#include <ImGuiPack.h>

#include <SoGLSL/src/Gui/CustomGuiWidgets.h>
#include <SoGLSL/src/Buffer/FrameBuffersPipeLine.h>
#include <SoGLSL/src/Renderer/RenderPack.h>

#include <Backends/MainBackend.h>

#define DEBUG_LINE_HISTO_COUNT 200

PixelDebugLine::PixelDebugLine()
{
	puNeedRefresh = false;

	puGraphHoveredIdx = -1;
	puCurrentAttachment = 0;
	
	puCountPointToShowOnGraph = DEBUG_LINE_HISTO_COUNT - 1;
	puValuesHisto = new ct::fvec4[DEBUG_LINE_HISTO_COUNT];
	puGraphLimits = ct::fvec2(0.0f, 1.0f);
	puImagePixelDisplay = 5;
	puShowImage = false;

	puLimitType = PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_AUTO;
	puPixelDebugMeasureLimitTypeIdx= (int)puLimitType;
	
	for (auto j = 0; j < 4; j++)
	{
		auto res = true;
		if (j == 3) res = false;
		puShowLine[j] = res;
	}

	puIsMousePoint = false;

	InitBufferVariables();
}

PixelDebugLine::~PixelDebugLine()
{
	SAFE_DELETE_ARRAY(puValuesHisto);
}

void PixelDebugLine::Init(RenderPackWeak vRenderPack)
{
	puActivated = false;
	CreateBuffer(vRenderPack, puFirstPos, puLastPos);
}

void PixelDebugLine::DrawMenu()
{
	ImGui::Checkbox("Under Line", &puActivated);
}

void PixelDebugLine::SetMousePointType(bool vIsMousePoint)
{
	puIsMousePoint = vIsMousePoint;
}

bool PixelDebugLine::IsMousePointType()
{
	return puIsMousePoint;
}

void PixelDebugLine::DrawTooltips(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* /*vCamera*/)
{
	if (!puActivated) return;

	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		auto& g = *GImGui;
		
		if (puCapturesPoints.size() && puGraphHoveredIdx >= 0)
		{
			puGraphHoveredIdx = puGraphHoveredIdx % puCapturesPoints.size();
		}
		auto idx = 0;
		for (auto it = puCapturesPoints.begin(); it != puCapturesPoints.end(); ++it)
		{
			auto p = *it;
			p.x = (p.x + puBufferAABB.lowerBound.x) * rpPtr->puMeshRect.w + rpPtr->puMeshRect.x;
			p.y = (1.0f - (p.y + puBufferAABB.lowerBound.y)) * rpPtr->puMeshRect.h + rpPtr->puMeshRect.y;
			p *= vDisplayQuality;
			if (g.CurrentViewport)
			{
				if (puGraphHoveredIdx == idx)
				{
					ImGui::GetCurrentWindow()->DrawList->AddRectFilled(
						g.CurrentViewport->Pos + ImVec2(p.x - 5.0f, p.y - 5.0f), 
						g.CurrentViewport->Pos + ImVec2(p.x + 5.0f, p.y + 5.0f),
						ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
				}
				else
				{
					ImGui::GetCurrentWindow()->DrawList->AddRectFilled(
						g.CurrentViewport->Pos + ImVec2(p.x - 2.0f, p.y - 2.0f), 
						g.CurrentViewport->Pos + ImVec2(p.x + 2.0f, p.y + 2.0f),
						ImGui::GetColorU32(ImVec4(1.0f - p.z, p.z, 0.0f, 1.0f)));
				}
			}
			idx++;
		}

		if (g.CurrentViewport)
		{
			const auto p0 = g.CurrentViewport->Pos + ImVec2(puFirstPos.x * rpPtr->puMeshRect.w + rpPtr->puMeshRect.x, (1.0f - puFirstPos.y) * rpPtr->puMeshRect.h + rpPtr->puMeshRect.y) * vDisplayQuality;
			const auto p1 = g.CurrentViewport->Pos + ImVec2(puLastPos.x * rpPtr->puMeshRect.w + rpPtr->puMeshRect.x, (1.0f - puLastPos.y) * rpPtr->puMeshRect.h + rpPtr->puMeshRect.y) * vDisplayQuality;
			ImGui::GetCurrentWindow()->DrawList->AddLine(p0, p1, ImGui::GetColorU32(ImVec4(1, 1, 0, 1)));
		}

		const auto mi = MainBackend::Instance()->puMouseInterface;
		const ct::fvec2 mousePos(mi.px, mi.py);
		const auto pos = mousePos / vDisplayQuality;
		if (rpPtr->puMeshRect.IsContainPoint(pos))
		{
			if (puPointToPlace > 0)
			{
				if (mi.buttonDown[0]) // left button
				{
					puFirstPos.x = (pos.x - rpPtr->puMeshRect.x) / rpPtr->puMeshRect.w;
					puFirstPos.y = 1.0f - (pos.y - rpPtr->puMeshRect.y) / rpPtr->puMeshRect.h;

					CreateBuffer(vRenderPack, puFirstPos, puLastPos);
				}

				if (mi.buttonDown[1]) // right
				{
					puLastPos.x = (pos.x - rpPtr->puMeshRect.x) / rpPtr->puMeshRect.w;
					puLastPos.y = 1.0f - (pos.y - rpPtr->puMeshRect.y) / rpPtr->puMeshRect.h;

					CreateBuffer(vRenderPack, puFirstPos, puLastPos);
				}
			}
		}
	}
}

void PixelDebugLine::DrawPane(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem* /*vCamera*/)
{
	if (!puActivated) return;

	auto rpPtr = vRenderPack.lock();
	if (rpPtr)
	{
		if (ImGui::CollapsingHeader("Under Line"))
		{
			if (ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), "Edit##editpoints", "Edit point", puPointToPlace > 0)) puPointToPlace = 1 - puPointToPlace;
			
			if (puPointToPlace)
			{
				ImGui::SameLine();
				ImGui::Text("Mouse Left => Define Start Point\nMouse Right => Define End Point");
			}

			ImGui::Text("Start : x:%.5f y:%.5f", puFirstPos.x, puFirstPos.y);
			ImGui::Text("End : x:%.5f y:%.5f", puLastPos.x, puLastPos.y);

			char buf[2] = "";
			char bufHelp[100] = "";
			for (auto i = 0; i <= rpPtr->puZeroBasedMaxSliceBufferId; i++)
			{
				snprintf(buf, 2, "%i", i);
				snprintf(bufHelp, 100, "Use Attachment %i", i);
				if (i > 0)
					ImGui::SameLine();
				if (ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), buf, bufHelp, i == puCurrentAttachment))
				{
					puCurrentAttachment = i;
				}
			}

			DrawGraph(puCurrentAttachment, vRenderPack, vDisplayQuality);
		}
	}
}

void PixelDebugLine::Capture(RenderPackWeak vRenderPack, float /*vDisplayQuality*/, CameraSystem* /*vCamera*/)
{
	if (!puActivated) return;

	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		if (puFirstPos >= 0.0f && puLastPos <= 1.0f)
		{
			puCurrentAttachment = ct::clamp(puCurrentAttachment, 0, rpPtr->puZeroBasedMaxSliceBufferId);
			CaptureFromGPU(vRenderPack, puCurrentAttachment);
			PrepareDatas(vRenderPack, puCurrentAttachment);
		}
	}
}

void PixelDebugLine::Resize(RenderPackWeak vRenderPack, float /*vDisplayQuality*/, CameraSystem* /*vCamera*/)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		CreateBuffer(vRenderPack, puFirstPos, puLastPos);
	}
}

bool PixelDebugLine::NeedRefresh()
{
	return puNeedRefresh;
}

/////////////////////////////////////////////////////////////
/////// PRIVATE /////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void PixelDebugLine::SaveBufferToFile(const std::string& vFile)
{
	if (puBuffer && !puBufferSize.emptyAND())
	{
		std::ofstream fileWriter(vFile, std::ios::out);
		if (fileWriter.bad() == false)
		{
			const auto w = puBufferSize.x;
			const auto h = puBufferSize.y;

			for (auto y = 0; y < h; y++)
			{
				std::string row;
				for (auto x = 0; x < w; x++)
				{
					const auto i = y * w * puRGBAOffset + x * puRGBAOffset;

					if (i >= 0 && i < puBufferLength)
					{
						if (puBuffer[i + 0] > 0.5f)
							row += "1";
						else
							row += "0";
						//row += ct::toStr(floorf(puBuffer[i + 0] * 100.0f) / 100.0f) + ";";
						//row += ct::toStr(floorf(puBuffer[i + 1] * 100.0f) / 100.0f) + ";";
						//row += ct::toStr(floorf(puBuffer[i + 2] * 100.0f) / 100.0f) + ";";
						//row += ct::toStr(floorf(puBuffer[i + 3] * 100.0f) / 100.0f) + ";";
					}
					else
					{
						row += "-";
						//row += "nok(x:" + ct::toStr(x) + ",y:" + ct::toStr(y) + ",i:" + ct::toStr(i) + ");";
					}
				}
				fileWriter << row << "\n";
			}

			fileWriter.close();
		}
	}
}

void PixelDebugLine::InitBufferVariables()
{
	puPointToPlace = 0;
	puFirstPos = 0;
	puLastPos = 0;
	puBufferAABB = ct::fAABB();
	puIterStartPos = 0;
	puIterDirection = 0;
	puBuffer = nullptr;
	puBufferLength = 0;
	puRGBAOffset = 4;
}

// vStartPos et vEndPos sont en cooredonn�es normalis�es
void PixelDebugLine::CreateBuffer(RenderPackWeak vRenderPack, ct::fvec2 vStartPos, ct::fvec2 vEndPos)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		if (vStartPos > 0.0f && vEndPos < 1.0f && 
			IS_FLOAT_DIFFERENT(vStartPos.x, vEndPos.x) && 
			IS_FLOAT_DIFFERENT(vStartPos.y, vEndPos.y))
		{
			DestroyBuffer();

			puBufferAABB.Set(vStartPos, vEndPos);
			
			// valeurs locales relatif au AABB
			puIterStartPos = vStartPos - puBufferAABB.lowerBound;
			puIterDirection = (vEndPos - vStartPos).GetNormalized(); // on inverse le y

			const ct::fvec2 fboSize((float)rpPtr->GetPipe()->size.x, (float)rpPtr->GetPipe()->size.y);

			const auto pos = puBufferAABB.lowerBound * fboSize;
			puCapturePos.x = (int)pos.x;
			puCapturePos.y = (int)pos.y;

			const auto size = puBufferAABB.Size() * fboSize;
			puBufferSize.x = (int)size.x;
			puBufferSize.y = (int)size.y;
			puBufferLength = puBufferSize.x * puBufferSize.y * puRGBAOffset + 1;

			puBuffer = new float[puBufferLength];
		}
	}
}

void PixelDebugLine::DestroyBuffer()
{
	SAFE_DELETE_ARRAY(puBuffer);
	puBufferLength = 0;
	puBufferAABB = ct::fAABB();
}

void PixelDebugLine::CaptureFromGPU(RenderPackWeak vRenderPack, int vAttachmentId)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		const auto fboSize = rpPtr->GetPipe()->size.xy();
		
		if (puBuffer && !puBufferSize.emptyAND() && puCapturePos >= 0)
		{
			if (puCapturePos.x + puBufferSize.x < fboSize.x &&
				puCapturePos.y + puBufferSize.y < fboSize.y)
			{
				GuiBackend::MakeContextCurrent(rpPtr->puWindow);

				if (rpPtr->BindFBO())
				{
					glReadBuffer(GL_COLOR_ATTACHMENT0 + vAttachmentId);
					LogGlError();
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // from disk to opengl
					LogGlError();
					glPixelStorei(GL_PACK_ALIGNMENT, 1); // from opengl to disk
					LogGlError();
					glReadPixels(puCapturePos.x, puCapturePos.y, puBufferSize.x, puBufferSize.y, GL_RGBA, GL_FLOAT, puBuffer);
					LogGlError();

					rpPtr->UnBindFBO(false, false);
				}
			}
		}
		else
		{
			CreateBuffer(vRenderPack, puFirstPos, puLastPos);
			// on fera la maj dans la prochaine frame
		}
	}
}

// on parcours la diagonale du buffer
// on stocke les valeurs en fonction de DEBUG_LINE_HISTO_COUNT
void PixelDebugLine::PrepareDatas(RenderPackWeak vRenderPack, int /*vAttachmentId*/)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		auto fboSize = rpPtr->GetPipe()->size.xy();
		if (puBuffer && !puBufferSize.emptyAND())
		{
			const auto zbs = (double)puCountPointToShowOnGraph; // zero based size

			auto size = puBufferAABB.Size();
			const auto len = (double)size.length();
			const auto step = len / zbs;

			auto ro = ct::dvec2((double)puIterStartPos.x, (double)puIterStartPos.y);
			const auto rds = ct::dvec2((double)puIterDirection.x, (double)puIterDirection.y) * step;
			
			puCapturesPoints.clear();

			const auto count = (int)zbs;
			for (auto i = 0; i <= count; i++)
			{
				if (ro >= 0.0)
				{
					const auto x = (int)ct::floor(ro.x * (puBufferSize.x-1) / size.x);
					const auto y = (int)ct::floor(ro.y * (puBufferSize.y-1) / size.y);

					const auto idx = x * puRGBAOffset + y * puBufferSize.x * puRGBAOffset;

					if (idx >= 0 && idx < puBufferLength)
					{
						puValuesHisto[i].x = puBuffer[idx + 0];
						puValuesHisto[i].y = puBuffer[idx + 1];
						puValuesHisto[i].z = puBuffer[idx + 2];
						puValuesHisto[i].w = puBuffer[idx + 3];

						if (puLimitType == PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_MIN_MAX_HISTO)
						{
							if (puShowLine[0])
							{
								puGraphLimits.x = ct::mini(puGraphLimits.x, puValuesHisto[i].x);
								puGraphLimits.y = ct::maxi(puGraphLimits.y, puValuesHisto[i].x);
							}
							if (puShowLine[1])
							{
								puGraphLimits.x = ct::mini(puGraphLimits.x, puValuesHisto[i].y);
								puGraphLimits.y = ct::maxi(puGraphLimits.y, puValuesHisto[i].y);
							}
							if (puShowLine[2])
							{
								puGraphLimits.x = ct::mini(puGraphLimits.x, puValuesHisto[i].z);
								puGraphLimits.y = ct::maxi(puGraphLimits.y, puValuesHisto[i].z);
							}
							if (puShowLine[3])
							{
								puGraphLimits.x = ct::mini(puGraphLimits.x, puValuesHisto[i].w);
								puGraphLimits.y = ct::maxi(puGraphLimits.y, puValuesHisto[i].w);
							}
						}

						ct::fvec3 pt((float)ro.x, (float)ro.y, 1.0f); // 0.0f => bon
						puCapturesPoints.emplace_back(pt);
					}
					else
					{
						ct::fvec3 pt((float)ro.x, (float)ro.y, 0.0f); // 0.0f => mauvais
						puCapturesPoints.emplace_back(pt);

						break;
					}
				}
				else
				{
					ct::fvec3 pt((float)ro.x, (float)ro.y, 0.0f); // 0.0f => mauvais
					puCapturesPoints.emplace_back(pt);

					break;
				}

				ro += rds;
			}
		}
	}
}

void PixelDebugLine::DrawGraph(size_t vAttachment, RenderPackWeak vRenderPack, float /*vDisplayQuality*/)
{
	auto rpPtr = vRenderPack.lock();
	if (vAttachment <= rpPtr->puZeroBasedMaxSliceBufferId)
	{
		char buffer[50] = "\0";
		snprintf(buffer, 50, "Attachment %u", (uint32_t)vAttachment);

		ImGui::BeginFramedGroup(buffer);

		if (ImGui::ContrastedCombo(150, "Limits", &puPixelDebugMeasureLimitTypeIdx, "AUTO\0CUSTOM\0MIN MAX HISTO\0\0", -1))
		{
			puLimitType = (PixelDebugMeasureLimitType)puPixelDebugMeasureLimitTypeIdx;

			if (puLimitType == PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_MIN_MAX_HISTO)
			{
				puGraphLimits.x = 0;
				puGraphLimits.y = 0;
			}
		}

		if (puLimitType == PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_AUTO)
		{
			puGraphLimits.x = FLT_MAX;
			puGraphLimits.y = FLT_MAX;
		}
		else if (puLimitType == PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_CUSTOM)
		{
			ImGui::InputFloatDefault(70.0f, "##MinY", &puGraphLimits.x, 0.0f); ImGui::SameLine();
			ImGui::InputFloatDefault(70.0f, "##MaxY", &puGraphLimits.y, 1.0f);
		}

		ImGui::BeginGroup();
		ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), "R##SHOW_RED_CHANNEL", "Show Red Channel", &puShowLine[0]);
		ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), "G##SHOW_GREEN_CHANNEL", "Show Green Channel", &puShowLine[1]);
		ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), "B##SHOW_BLUE_CHANNEL", "Show Blue Channel", &puShowLine[2]);
		ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), "A##SHOW_ALPHA_CHANNEL", "Show Alpha Channel", &puShowLine[3]);
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::PlotFVec4Histo("##SHOW_CHANNEL", puValuesHisto, puCountPointToShowOnGraph,
			puShowLine, ImVec2(200, 92),
			puGraphLimits.x, puGraphLimits.y, &puGraphHoveredIdx);
		if (ImGui::SliderIntDefault(200, "Histo", &puCountPointToShowOnGraph, 3, DEBUG_LINE_HISTO_COUNT - 1, DEBUG_LINE_HISTO_COUNT - 1))
		{
			puCountPointToShowOnGraph = ct::clamp(puCountPointToShowOnGraph, 3, DEBUG_LINE_HISTO_COUNT - 1);
			puNeedRefresh = true;
		}

		/*ImGui::CheckBoxBoolDefault("Show Pixel Preview", &puShowImage, true);
		if (puShowImage)
		{
			ImGui::SliderIntDefault(150, "Pixels", &puImagePixelDisplay, 1, 20, 5);
			ImVec2 a = ImVec2(puFirstPos.x, puFirstPos.y);
			ImVec2 b = ImVec2(puLastPos.x, puLastPos.y);
			ImGui::ImageZoomLine((ImTextureID)vRenderPack->GetTextureId(vAttachment), 200, a, b);
		}*/

		ImGui::EndFramedGroup();
	}
}

std::string PixelDebugLine::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + "<active>" + ct::toStr(puActivated ? "true" : "false") + "</active>\n";
	str += vOffset + "<attachments value=\"" + ct::toStr(puCurrentAttachment) + "\">\n";
	str += vOffset + "\t<channels value=\"";
	for (auto c = 0; c < 4; c++)
	{
		if (c > 0)
			str += ";";
		str += ct::toStr(puShowLine[c] ? "1" : "0");
	}
	str += "\"/>\n";;
	str += vOffset + "\t<limits value=\"" + puGraphLimits.string() + "\" type=\"" + ct::toStr(puLimitType) + "\"/>\n";
	str += vOffset + "\t<histo value=\"" + ct::toStr(puCountPointToShowOnGraph) + "\"/>\n";

	str += vOffset + "\t<line>\n";
	str += vOffset + "\t\t<start value=\"" + puFirstPos.string() + "\"/>\n";
	str += vOffset + "\t\t<end value=\"" + puLastPos.string() + "\"/>\n";
	str += vOffset + "\t</line>\n";
	str += vOffset + "</attachments>\n";

	return str;
}

bool PixelDebugLine::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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
	
	if (strName == "attachments")
	{
		auto att = vElem->FirstAttribute();
		if (att)
		{
			strName = att->Name();
			if (strName == "value")
			{
				strValue = att->Value();
				puCurrentAttachment = ct::ivariant(strValue).GetI();
			}
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
				ct::ivec4 v(strValue, ';', 4);
				puShowLine[0] = (bool)v.x;
				puShowLine[1] = (bool)v.y;
				puShowLine[2] = (bool)v.z;
				puShowLine[3] = (bool)v.w;
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
				puGraphLimits = ct::fvariant(attValue).GetV2();
			}
			if (attName == "type")
			{
				puPixelDebugMeasureLimitTypeIdx = ct::ivariant(attValue).GetI();
				puLimitType = (PixelDebugMeasureLimitType)puPixelDebugMeasureLimitTypeIdx;

				if (puLimitType == PixelDebugMeasureLimitType::PIXEL_DEBUG_MEASURE_LIMIT_MIN_MAX_HISTO)
				{
					puGraphLimits.x = 0;
					puGraphLimits.y = 0;
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
				puCountPointToShowOnGraph = ct::ivariant(strValue).GetI();
			}
		}
	}

	if (strName == "start" && strParentName == "line")
	{
		auto att = vElem->FirstAttribute();
		if (att)
		{
			strName = att->Name();
			if (strName == "value")
			{
				strValue = att->Value();
				puFirstPos = ct::fvariant(strValue).GetV2();
			}
		}
	}

	if (strName == "end" && strParentName == "line")
	{
		auto att = vElem->FirstAttribute();
		if (att)
		{
			strName = att->Name();
			if (strName == "value")
			{
				strValue = att->Value();
				puLastPos = ct::fvariant(strValue).GetV2();
			}
		}
	}

	return true;
}