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


#include "PathSystem.h"

#include <ctools/FileHelper.h>
#include <ctools/cTools.h>
#include <ImGuiPack.h>
#include <Headers/NoodlesPlateBuild.h>
#include <Backends/MainBackend.h>

///////////////////////////////////////////////////////
//// PathField ////////////////////////////////////////
///////////////////////////////////////////////////////

void PathField::SetUse(const std::string& vUse)
{
	m_Original_Use = vUse;
}

void PathField::SetPath(const std::string& vPath)
{
	if (!vPath.empty())
	{
		m_Original_Path = vPath;

#ifdef MSVC
		strncpy_s(m_New_Path_Buffer, vPath.c_str(), ct::mini<size_t>(2047U, vPath.size()));
#else
		strncpy(m_New_Path_Buffer, vPath.c_str(), ct::mini<size_t>(2047U, vPath.size()));
#endif
	}
}

std::string PathField::GetPath()
{
	return std::string(m_New_Path_Buffer, strlen(m_New_Path_Buffer));
}

void PathField::Draw(const char* vButtonLabel)
{
	if (ImGui::ContrastedButton(vButtonLabel, nullptr, nullptr, 250.0f)) {
        IGFD::FileDialogConfig config;
        config.path = m_Original_Path;
        config.countSelectionMax = 1;
        config.flags =  ImGuiFileDialogFlags_Modal;
        ImGuiFileDialog::Instance()->OpenDialog(vButtonLabel, "Select path", nullptr, config);
	}

	ImGui::SameLine();

	ImGui::PushID(vButtonLabel);
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputText("##PathField", m_New_Path_Buffer, 2047U);
	ImGui::PopItemWidth();
	ImGui::PopID();

	ImVec2 min = ImVec2(0, 0);
	ImVec2 max = ImVec2(FLT_MAX, FLT_MAX);
	if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
	{
		max = ImVec2((float)MainBackend::Instance()->puScreenSize.x, (float)MainBackend::Instance()->puScreenSize.y);
		min = max * 0.5f;
	}

	if (ImGuiFileDialog::Instance()->Display(vButtonLabel,
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking, min, max))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			auto path = ImGuiFileDialog::Instance()->GetCurrentPath();
			path = FileHelper::Instance()->GetPathRelativeToApp(path);
			path = FileHelper::Instance()->SimplifyFilePath(path);
			SetPath(path);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

// Apply change
void PathField::Apply()
{
	m_Original_Path = GetPath();
}

std::string PathField::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + "<PathField use=\"" + m_Original_Use + "\">" + GetPath() + "</PathField>\n";

	return str;
}

bool PathField::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

	if (strName == "PathField")
	{
		std::string use;

		for (auto attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next())
		{
			std::string attName = attr->Name();
			std::string attValue = attr->Value();

			if (attName == "use")
			{
				use = attValue;
				break;
			}
		}

		if (use == m_Original_Use)
		{
			SetPath(strValue);
		}
	}

	return false; // continue for explore childs. need to return false if we want explore child ourselves
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

PathSystem::PathSystem()
{
	m_PathField_Scripts.SetUse("FILE_LOCATION_SCRIPT");
	m_PathField_Assets_General.SetUse("FILE_LOCATION_ASSET_GENERAL");
	m_PathField_Assets_Texture_2D.SetUse("FILE_LOCATION_ASSET_TEXTURE_2D");
	m_PathField_Assets_Texture_3D.SetUse("FILE_LOCATION_ASSET_TEXTURE_3D");
	m_PathField_Assets_CubeMap.SetUse("FILE_LOCATION_ASSET_CUBEMAP");
#ifdef USE_SOUNDS
	m_PathField_Assets_Sounds.SetUse("FILE_LOCATION_ASSET_SOUND");
#endif
	m_PathField_Assets_Meshs.SetUse("FILE_LOCATION_ASSET_MESH");
	m_PathField_Assets_Meshs_Export.SetUse("FILE_LOCATION_EXPORT");
}

PathSystem::~PathSystem()
{
	
}

void PathSystem::Load()
{
	m_PathField_Scripts.SetPath(FileHelper::Instance()->GetRegisteredPath((int)FILE_LOCATION_Enum::FILE_LOCATION_SCRIPT));
	m_PathField_Assets_General.SetPath(FileHelper::Instance()->GetRegisteredPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_GENERAL));
	m_PathField_Assets_Texture_2D.SetPath(FileHelper::Instance()->GetRegisteredPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_TEXTURE_2D));
	m_PathField_Assets_Texture_3D.SetPath(FileHelper::Instance()->GetRegisteredPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_TEXTURE_3D));
	m_PathField_Assets_CubeMap.SetPath(FileHelper::Instance()->GetRegisteredPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_CUBEMAP));
#ifdef USE_SOUNDS
	m_PathField_Assets_Sounds.SetPath(FileHelper::Instance()->GetRegisteredPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_SOUND));
#endif
	m_PathField_Assets_Meshs.SetPath(FileHelper::Instance()->GetRegisteredPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_MESH));
	m_PathField_Assets_Meshs_Export.SetPath(FileHelper::Instance()->GetRegisteredPath((int)FILE_LOCATION_Enum::FILE_LOCATION_EXPORT));
}

void PathSystem::Save()
{
	m_PathField_Scripts.Apply();
	m_PathField_Assets_General.Apply();
	m_PathField_Assets_Texture_2D.Apply();
	m_PathField_Assets_Texture_3D.Apply();
	m_PathField_Assets_CubeMap.Apply();
#ifdef USE_SOUNDS
	m_PathField_Assets_Sounds.Apply();
#endif
	m_PathField_Assets_Meshs.Apply();
	m_PathField_Assets_Meshs_Export.Apply();

	FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_SCRIPT, m_PathField_Scripts.GetPath());
	FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_GENERAL, m_PathField_Assets_General.GetPath());
	FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_TEXTURE_2D, m_PathField_Assets_Texture_2D.GetPath());
	FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_TEXTURE_3D, m_PathField_Assets_Texture_3D.GetPath());
	FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_CUBEMAP, m_PathField_Assets_CubeMap.GetPath());
#ifdef USE_SOUNDS
	FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_SOUND, m_PathField_Assets_Sounds.GetPath());
#endif
	FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_ASSET_MESH, m_PathField_Assets_Meshs.GetPath());
	FileHelper::Instance()->RegisterPath((int)FILE_LOCATION_Enum::FILE_LOCATION_EXPORT , m_PathField_Assets_Meshs_Export.GetPath());

	// re set the path to track
	MainBackend::Instance()->InitFilesTracker();
}

void PathSystem::Draw()
{
	m_PathField_Scripts.Draw("Select path for Scripts");
	m_PathField_Assets_General.Draw("Select path for Generals Assets");
	m_PathField_Assets_Texture_2D.Draw("Select path for Texture 2D Assets");
	m_PathField_Assets_Texture_3D.Draw("Select path for Texture 3D Assets");
	m_PathField_Assets_CubeMap.Draw("Select path for CubeMaps Assets");
#ifdef USE_SOUNDS
	m_PathField_Assets_Sounds.Draw("Select path for Sounds Assets");
#endif
	m_PathField_Assets_Meshs.Draw("Select path for Meshs Assets");
	m_PathField_Assets_Meshs_Export.Draw("Select path for Meshs Export Assets");
}

std::string PathSystem::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + "<PathSystem>\n";
	str += m_PathField_Scripts.getXml(vOffset + "\t");
	str += m_PathField_Assets_General.getXml(vOffset + "\t");
	str += m_PathField_Assets_Texture_2D.getXml(vOffset + "\t");
	str += m_PathField_Assets_Texture_3D.getXml(vOffset + "\t");
	str += m_PathField_Assets_CubeMap.getXml(vOffset + "\t");
#ifdef USE_SOUNDS
	str += m_PathField_Assets_Sounds.getXml(vOffset + "\t");
#endif
	str += m_PathField_Assets_Meshs.getXml(vOffset + "\t");
	str += m_PathField_Assets_Meshs_Export.getXml(vOffset + "\t");
	str += vOffset + "</PathSystem>\n";
	
	return str;
}

bool PathSystem::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

	if (strParentName == "PathSystem")
	{
		m_PathField_Scripts.setFromXml(vElem, vParent, vUserDatas);
		m_PathField_Assets_General.setFromXml(vElem, vParent, vUserDatas);
		m_PathField_Assets_Texture_2D.setFromXml(vElem, vParent, vUserDatas);
		m_PathField_Assets_Texture_3D.setFromXml(vElem, vParent, vUserDatas);
		m_PathField_Assets_CubeMap.setFromXml(vElem, vParent, vUserDatas);
#ifdef USE_SOUNDS
		m_PathField_Assets_Sounds.setFromXml(vElem, vParent, vUserDatas);
#endif
		m_PathField_Assets_Meshs.setFromXml(vElem, vParent, vUserDatas);
		m_PathField_Assets_Meshs_Export.setFromXml(vElem, vParent, vUserDatas);
	}

	return true; // continue for explore childs. need to return false if we want explore child ourselves
}