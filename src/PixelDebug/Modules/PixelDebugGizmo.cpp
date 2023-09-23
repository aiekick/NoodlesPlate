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


#include "PixelDebugGizmo.h"
#include <ImGuiPack.h>

#include <SoGLSL/src/Gui/CustomGuiWidgets.h>
#include <SoGLSL/src/Renderer/RenderPack.h>
#include <SoGLSL/src/Systems/CameraSystem.h>


#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>

/*
le but est de debuge des position en positionnat les gizmo de la scene:
il faudra affetcer un ou luisuers des gizmo dispo dans la scene :
- soit on positionne le gizmo a la valeur de position renvoy� par le buffer sous la souris
- soit on choisi les texel du buffer
- il faut aussi choisir le buffer
*/

PixelDebugGizmo::PixelDebugGizmo()
{
	pulastParsedAttachmentId = -1;
	puNeedRefresh = false;
	puUseUnderMouse = true;
	puAttachment_Gizmo = -1;
	puGizmo = nullptr;
	puActivated = false;
}

PixelDebugGizmo::~PixelDebugGizmo()
{

}

void PixelDebugGizmo::Init(RenderPackWeak  /*vRenderPack*/)
{
	puActivated = false;
}

void PixelDebugGizmo::DrawMenu()
{
	ImGui::Checkbox("Point 3d Via Gizmo", &puActivated);
}

void PixelDebugGizmo::DrawTooltips(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem *vCamera)
{
	if (!puActivated) return;

	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe() && MainFrame::sCentralWindowHovered)
	{
		if (puUseUnderMouse)
		{
			const auto mi = MainBackend::Instance()->puMouseInterface;
			const ct::fvec2 mousePos(mi.px, mi.py);
			auto pos = mousePos / vDisplayQuality;
			if (rpPtr->puMeshRect.IsContainPoint(pos))
			{
				if (puAttachment_Gizmo > -1)
				{
					if (puGizmo && vCamera)
					{
						pos.x = (pos.x - rpPtr->puMeshRect.x) / rpPtr->puMeshRect.w;
						pos.y = 1.0f - (pos.y - rpPtr->puMeshRect.y) / rpPtr->puMeshRect.h;

						const auto point = puValues[puAttachment_Gizmo].xyz();

						auto p = ImVec2(pos.x * rpPtr->puMeshRect.w + rpPtr->puMeshRect.x, (1.0f - pos.y) * rpPtr->puMeshRect.h + rpPtr->puMeshRect.y);
						p.x -= rpPtr->puMeshRect.w * 0.5f;
						p.y -= rpPtr->puMeshRect.h * 0.5f;
						p *= vDisplayQuality;

						puGizmo->mat4[3] = glm::vec4(point.x, point.y, point.z, 1.0f);

						ImGui::BeginTooltip();
						ImGui::Text("Texel 2D : x:%.3f y:%.3f\nPoint 3D : x:%.3f y:%.3f z:%.3f\nDisplayed on %s",
							p.x, p.y,
							point.x, point.y, point.z,
							puGizmoName.c_str());
						ImGui::EndTooltip();

						puNeedRefresh = true;
					}
				}
			}
		}
		else
		{
			if (puAttachment_Gizmo > -1)
			{
				if (puGizmo && vCamera)
				{
					const auto point = puValues[puAttachment_Gizmo].xyz();

					puGizmo->mat4[3] = glm::vec4(point.x, point.y, point.z, 1.0f);

					ImGui::BeginTooltip();
					ImGui::Text("Point 3D : x:%.3f y:%.3f z:%.3f\nDisplayed on %s",
						point.x, point.y, point.z,
						puGizmoName.c_str());
					ImGui::EndTooltip();

					puNeedRefresh = true;
				}
			}
		}
	}
}

void PixelDebugGizmo::DrawPane(RenderPackWeak vRenderPack, float /*vDisplayQuality*/, CameraSystem * /*vCamera*/)
{
	if (!puActivated) return;

	auto rpPtr = vRenderPack.lock();
	if (rpPtr)
	{
		if (ImGui::CollapsingHeader("Point 3D Debug via Gizmo"))
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Buffer Ids :");
			char buffer[10];

			for (auto i = 0; i <= rpPtr->puZeroBasedMaxSliceBufferId; i++)
			{
				if (i > 0)
					ImGui::SameLine();
				snprintf(buffer, 9, "%i", i);
				if (ImGui::RadioButtonLabeled(ImVec2(0.0f, 0.0f), buffer, buffer, puAttachment_Gizmo == i))
				{
					puAttachment_Gizmo = i;
				}
			}

			if (rpPtr->GetShaderKey())
			{
				auto lstGizmo = rpPtr->GetShaderKey()->GetUniformsByWidget("gizmo");
				if (lstGizmo)
				{
					if (ImGui::BeginListBox("Gizmo's", ImVec2(0, lstGizmo->size() * ImGui::GetTextLineHeight())))
					{
						for (auto it = lstGizmo->begin(); it != lstGizmo->end(); ++it)
						{
							if (ImGui::Selectable((*it)->name.c_str(), puGizmo == (*it)))
							{
								puGizmo = (*it);
								puGizmoName = puGizmo->name;
							}
						}

						ImGui::EndListBox();
					}
				}

				if (!puGizmo)
				{
					if (!puGizmoName.empty())
					{
						puGizmo = rpPtr->GetShaderKey()->GetUniformByName(puGizmoName);
					}
				}
			}

			ImGui::CheckBoxBoolDefault("Point Under Mouse ?", &puUseUnderMouse, false, "Use the point under mouse in selected attachment");
			
			if (!puUseUnderMouse)
			{
				ImGui::InputFloatDefault(100, "Texel x", &puTexel.x, 0.5f);
				ImGui::InputFloatDefault(100, "Texel y", &puTexel.y, 0.5f);
			}
		}
	}
}

void PixelDebugGizmo::Capture(RenderPackWeak vRenderPack, float vDisplayQuality, CameraSystem * /*vCamera*/)
{
	auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		ct::fvec2 pos;
		
		if (puUseUnderMouse)
		{
			const auto mi = MainBackend::Instance()->puMouseInterface;
			const ct::fvec2 mousePos(mi.px, mi.py);
			pos = mousePos / vDisplayQuality;

			if (rpPtr->puMeshRect.IsContainPoint(pos))
			{
				pos.x = (pos.x - rpPtr->puMeshRect.x) / rpPtr->puMeshRect.w;
				pos.y = 1.0f - (pos.y - rpPtr->puMeshRect.y) / rpPtr->puMeshRect.h;

				if (rpPtr->GetFBOValuesAtNormalizedPos(pos, puValues, puAttachment_Gizmo + 1, false))
				{

				}
			}
		}
		else
		{
			pos = puTexel;

			pos.y = 1.0f - pos.y;

			if (rpPtr->GetFBOValuesAtNormalizedPos(pos, puValues, puAttachment_Gizmo + 1, false))
			{

			}
		}
	}
}

void PixelDebugGizmo::Resize(RenderPackWeak vRenderPack, float /*vDisplayQualit*/, CameraSystem * /*vCameray*/)
{
	/*auto rpPtr = vRenderPack.lock();
	if (rpPtr && rpPtr->GetPipe())
	{
		
	}*/
}

bool PixelDebugGizmo::NeedRefresh()
{
	return puNeedRefresh;
}

void PixelDebugGizmo::DrawPoint(ct::fvec2 *vPoint, ct::frect *vRect, float vDisplayQuality)
{
	auto drawList = ImGui::GetWindowDrawList();

	const auto p = ImVec2(vPoint->x * vRect->w + vRect->x, (1.0f - vPoint->y) * vRect->h + vRect->y) * vDisplayQuality;

	drawList->AddCircleFilled(p, 5.0f, ImColor(100, 100, 0));
	drawList->AddCircle(p, 5.0f, ImColor(255, 255, 255), 12, 2.0f);
}

void PixelDebugGizmo::DrawLine(ct::fvec4 *vLine, ct::frect *vRect, float vDisplayQuality)
{
	auto drawList = ImGui::GetWindowDrawList();

	const auto p0 = ImVec2(vLine->x * vRect->w + vRect->x, (1.0f - vLine->y) * vRect->h + vRect->y) * vDisplayQuality;
	const auto p1 = ImVec2(vLine->z * vRect->w + vRect->x, (1.0f - vLine->w) * vRect->h + vRect->y) * vDisplayQuality;

	drawList->AddLine(p0, p1, ImColor(200, 200, 100), 2.0f);
}

/////////////////////////////////////////////////////////////
/////// PRIVATE /////////////////////////////////////////////
/////////////////////////////////////////////////////////////

std::string PixelDebugGizmo::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + "<active>" + ct::toStr(puActivated ? "true" : "false") + "</active>\n";
	str += vOffset + "<texel>" + puTexel.string() + "</texel>\n";
	str += vOffset + "<usemouse>" + ct::toStr(puUseUnderMouse ? "true" : "false") + "</usemouse>\n";
	str += vOffset + "<attachment>" + ct::toStr(puAttachment_Gizmo) + "</attachment>\n";
	str += vOffset + "<selectedgizmo>" + puGizmoName + "</selectedgizmo>\n";
	
	return str;
}

bool PixelDebugGizmo::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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
	if (strName == "texel")
		puTexel = ct::fvariant(strValue).GetV2();
	if (strName == "usemouse")
		puUseUnderMouse = ct::ivariant(strValue).GetB();
	if (strName == "attachment")
		puAttachment_Gizmo = ct::ivariant(strValue).GetI();
	if (strName == "selectedgizmo")
		puGizmoName = strValue;

	return true;
}