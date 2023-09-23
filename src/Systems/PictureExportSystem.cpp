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


#include "PictureExportSystem.h"

// SoGlSL/3rdparty
#include <ctools/FileHelper.h>
#include <ctools/Logger.h>

// SoGlSL
#include <SoGLSL/src/Renderer/RenderPack.h>
#include <SoGLSL/src/CodeTree/ShaderKey.h>
#include <SoGLSL/src/Buffer/FrameBuffer.h>
#include <SoGLSL/src/Gui/CustomGuiWidgets.h>
#include <SoGLSL/src/Systems/ScreenGrabber.h>
#include <Panes/MessagePane.h>


// This project
#include <Backends/MainBackend.h>

PictureExportSystem::PictureExportSystem()
{
	
}

PictureExportSystem::~PictureExportSystem()
{
	
}

void PictureExportSystem::ScreenGrabbIfNeeded(const GuiBackend_Window& /*vWin*/)
{
	if (puTakePictureFromScreenNeeded)
	{
		auto displayShaderPtr = MainBackend::Instance()->puDisplay_RenderPack.lock();
		if (displayShaderPtr)
		{
			if (displayShaderPtr->GetShaderKey())
			{
				TakeScreenShotFromSceenWithTimeStamp(displayShaderPtr->GetShaderKey()->puKey);
			}
		}
		puTakePictureFromScreenNeeded = false;
	}
}

void PictureExportSystem::ExportRenderPackToPictureFile(
	RenderPackWeak vRP, const std::string& vFilePathName, int vBufferId)
{
	auto rpPtr = vRP.lock();
	if (rpPtr)
	{
		if (!vFilePathName.empty() && vBufferId >= 0)
		{
			if (vFilePathName.find(".png") != std::string::npos)
			{
				rpPtr->SaveFBOToPng(vFilePathName, puExport_Texture_FlipY, puExport_Texture_Sample_Count, ct::ivec2(0), vBufferId);
			}
			else if (vFilePathName.find(".bmp") != std::string::npos)
			{
				rpPtr->SaveFBOToBmp(vFilePathName, puExport_Texture_FlipY, puExport_Texture_Sample_Count, ct::ivec2(0), vBufferId);
			}
			else if (vFilePathName.find(".tga") != std::string::npos)
			{
				rpPtr->SaveFBOToTga(vFilePathName, puExport_Texture_FlipY, puExport_Texture_Sample_Count, ct::ivec2(0), vBufferId);
			}
			else if (vFilePathName.find(".hdr") != std::string::npos)
			{
				rpPtr->SaveFBOToHdr(vFilePathName, puExport_Texture_FlipY, puExport_Texture_Sample_Count, ct::ivec2(0), vBufferId);
			}
			else if (vFilePathName.find(".jpg") != std::string::npos)
			{
				rpPtr->SaveFBOToJpg(vFilePathName, puExport_Texture_FlipY, puExport_Texture_Sample_Count, 100, ct::ivec2(0), vBufferId);
			}
		}
	}
}

void PictureExportSystem::ExportFBOToPictureFile(
	FrameBufferPtr vFBO, const std::string& vFilePathName, int vBufferId)
{
	if (vFBO)
	{
		auto fboSize = vFBO->getSize().xy();
		if (!fboSize.emptyAND())
		{
			if (!vFilePathName.empty() && vBufferId >= 0)
			{
				if (vFilePathName.find(".png") != std::string::npos)
				{
					vFBO->SaveToPng(vFilePathName, puExport_Texture_FlipY, puExport_Texture_Sample_Count, fboSize, vBufferId);
				}
				else if (vFilePathName.find(".bmp") != std::string::npos)
				{
					vFBO->SaveToBmp(vFilePathName, puExport_Texture_FlipY, puExport_Texture_Sample_Count, fboSize, vBufferId);
				}
				else if (vFilePathName.find(".tga") != std::string::npos)
				{
					vFBO->SaveToTga(vFilePathName, puExport_Texture_FlipY, puExport_Texture_Sample_Count, fboSize, vBufferId);
				}
				else if (vFilePathName.find(".hdr") != std::string::npos)
				{
					vFBO->SaveToHdr(vFilePathName, puExport_Texture_FlipY, puExport_Texture_Sample_Count, fboSize, vBufferId);
				}
				else if (vFilePathName.find(".jpg") != std::string::npos)
				{
					vFBO->SaveToJpg(vFilePathName, puExport_Texture_FlipY, puExport_Texture_Sample_Count, 100, fboSize, vBufferId);
				}
			}
		}
	}
}

void PictureExportSystem::ExportPictureDialogOptions(std::string /*vFilter*/, IGFDUserDatas /*vUserDatas*/, bool *
                                                     /*vCantContinue*/)
{
	ImGui::Checkbox("Flip Y", &puExport_Texture_FlipY);
	ImGui::SliderIntDefault(100.0f, "Samples Count", &puExport_Texture_Sample_Count, 0, 4, 0);
}

bool PictureExportSystem::Is_TakeScreenSHotForEachModif_Needed()
{
	return 
		puTakePictureFromFBOForEachCodeUpdate || 
		puTakePictureFromScreenForEachCodeUpdate || 
		puAlwaysShowShotButton;
}

void PictureExportSystem::TakeScreenShotForEachModif(bool vForceShot)
{
	if (!MessagePane::Instance()->IsThereSomeMessages(true))
	{
		if (puTakePictureFromScreenForEachCodeUpdate || puTakePictureFromFBOForEachCodeUpdate || vForceShot)
		{
			MainBackend::Instance()->Render();
		}

		if (puTakePictureFromFBOForEachCodeUpdate || vForceShot)
		{
			auto displayShaderPtr = MainBackend::Instance()->puDisplay_RenderPack.lock();
			if (displayShaderPtr)
			{
				if (displayShaderPtr->GetShaderKey())
				{
					TakeScreenShotFromCurrentRenderPackWithTimeStamp(MainBackend::Instance()->puDisplay_RenderPack);
				}
			}
		}

		if (puTakePictureFromScreenForEachCodeUpdate || vForceShot)
		{
			puTakePictureFromScreenNeeded = true;
		}
	}
}

std::string PictureExportSystem::ModifyFilePathNameWithTimeStamp(const std::string& vFilePathName, std::string vExt)
{
	auto filePathName = vFilePathName;

	const auto extPos = filePathName.find(".glsl");
	if (extPos != std::string::npos)
	{
		filePathName = filePathName.substr(0, extPos);
		filePathName += "_";

		filePathName += FileHelper::Instance()->GetTimeStampToString();

		if (!vExt.empty())
		{
			filePathName += vExt;
		}

		return filePathName;
	}

	return "";
}

void PictureExportSystem::TakeScreenShotFromCurrentRenderPackWithTimeStamp(RenderPackWeak vRp)
{
	auto rpPtr = vRp.lock();
	if (rpPtr)
	{
		const auto file = ModifyFilePathNameWithTimeStamp(rpPtr->GetShaderKey()->puKey, "_FBO.png");
		if (!file.empty())
		{
			ExportRenderPackToPictureFile(MainBackend::Instance()->puDisplay_RenderPack, file, 0);
		}
		else
		{
			LogVarError("Can't take ScreenShot From Current FBO !");
		}
	}
}

void PictureExportSystem::TakeScreenShotFromSceenWithTimeStamp(const std::string& vFilePathName)
{
	if (!vFilePathName.empty())
	{
		const auto file = ModifyFilePathNameWithTimeStamp(vFilePathName, "_UI.png");
		if (!file.empty())
		{
			ScreenGrabber::Instance()->SaveToPng(MainBackend::sMainThread, file, 0, ct::fvec2());
		}
		else
		{
			LogVarError("Can't take ScreenShot From Current FBO !");
		}
	}
}

std::string PictureExportSystem::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;
	
	str += vOffset + "<PictureExportSystem>\n";
	const auto offset = vOffset + vOffset;

	str += offset + "<AlwaysShowShotButton>" + (puAlwaysShowShotButton ? "true" : "false") + "</AlwaysShowShotButton>\n";
	str += offset + "<TakePictureFromFBOForEachCodeUpdate>" + (puTakePictureFromFBOForEachCodeUpdate ? "true" : "false") + "</TakePictureFromFBOForEachCodeUpdate>\n";
	str += offset + "<TakePictureFromScreenForEachCodeUpdate>" + (puTakePictureFromScreenForEachCodeUpdate ? "true" : "false") + "</TakePictureFromScreenForEachCodeUpdate>\n";
	
	str += offset + "<exportfliptexture>" + (puExport_Texture_FlipY ? "true" : "false") + "</exportfliptexture>\n";
	str += offset + "<exportsamplescount>" + ct::toStr(puExport_Texture_Sample_Count) + "</exportsamplescount>\n";

	str += vOffset + "</PictureExportSystem>\n";

	return str;
}

bool PictureExportSystem::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

	if (strName == "PictureExportSystem")
	{
		for (auto child = vElem->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			RecursParsingConfig(child->ToElement(), vElem);
		}
	}

	// check� et utilis� au chargement de l'app
		//if (strName == "useTransparancy")
		//	StaticConfig::puUseTransparancy = ct::ivariant(strValue).GetB();
	if (strParentName == "PictureExportSystem")
	{
		if (strName == "exportfliptexture")
			puExport_Texture_FlipY = ct::ivariant(strValue).GetB();
		if (strName == "exportsamplescount")
			puExport_Texture_Sample_Count = ct::ivariant(strValue).GetI();

		if (strName == "AlwaysShowShotButton")
			puAlwaysShowShotButton = ct::ivariant(strValue).GetB();
		if (strName == "TakePictureFromFBOForEachCodeUpdate")
			puTakePictureFromFBOForEachCodeUpdate = ct::ivariant(strValue).GetB();
		if (strName == "TakePictureFromScreenForEachCodeUpdate")
			puTakePictureFromScreenForEachCodeUpdate = ct::ivariant(strValue).GetB();
	}

	return true;
}

void PictureExportSystem::DrawButton_Shot()
{
	if (Is_TakeScreenSHotForEachModif_Needed())
	{
		if (ImGui::ContrastedButton("Shot", "Fast ScreenShot According to Each Modif Settings"))
		{
			TakeScreenShotForEachModif(true);
		}
	}
}

void PictureExportSystem::DrawMenu()
{
	ImGui::Checkbox("From Current FBO", &puTakePictureFromFBOForEachCodeUpdate);
	ImGui::Checkbox("From Screen with UI", &puTakePictureFromScreenForEachCodeUpdate);
	ImGui::Checkbox("Always Show Shot Button", &puAlwaysShowShotButton);
}