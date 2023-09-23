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


#include "VersionSystem.h"

#include <ctools/FileHelper.h>
#include <ImGuiPack.h>

#include <SoGLSL/src/Gui/CustomGuiWidgets.h>
#include <SoGLSL/src/Importer/NetCodeRetriever.h>

#include <Headers/NoodlesPlateBuild.h>

///////////////////////////////////////////////////////
//// STATIC ///////////////////////////////////////////
///////////////////////////////////////////////////////

inline void MainFrame_LinkCallback(ImGui::MarkdownLinkCallbackData data_)
{
	const std::string url(data_.link, data_.linkLength);
	if (!data_.isImage)
	{
		FileHelper::Instance()->OpenUrl(url);
	}
}

inline ImGui::MarkdownImageData MainFrame_ImageCallback(ImGui::MarkdownLinkCallbackData /*data_*/)
{
	// In your application you would load an image based on data_ input. Here we just use the imgui font texture.
	const auto image = ImGui::GetIO().Fonts->TexID;
	const ImGui::MarkdownImageData imageData{ true, false, image, ImVec2(40.0f, 20.0f) };
	return imageData;
}

static ImGui::MarkdownConfig MainFrame_mdConfig{ MainFrame_LinkCallback, nullptr, MainFrame_ImageCallback, "", { {nullptr, true }, {nullptr, true }, {nullptr, false } } };

inline void MainFrame_Markdown(const std::string& markdown_)
{
	// fonts for, respectively, headings H1, H2, H3 and beyond
	ImGui::Markdown(markdown_.c_str(), markdown_.length(), MainFrame_mdConfig);
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

VersionSystem::VersionSystem()
{
	
}

VersionSystem::~VersionSystem()
{
	
}

void VersionSystem::CheckVersionAtStart()
{
	if (puNeedCheckVersionAtStart)
	{
		CheckLastVersion();
		puNeedCheckVersionAtStart = false;
	}
}

void VersionSystem::CheckLastVersion(bool vForce)
{
#ifdef USE_NETWORK
	if (vForce || NetCodeRetriever::Instance()->puCheckVersionAtStart)
	{
		puCheckVersionAtStart = !vForce;

		NetCodeRetriever::Instance()->RetrieveLastVersionWithoutThread(
			"https://raw.githubusercontent.com/aiekick/NoodlesPlate/master/VERSION",
			std::bind(&VersionSystem::VersionRetrieved, this));
	}
#else
	UNUSED(vForce);
#endif // #ifdef USE_NETWORK
}

void VersionSystem::ShowVersionChangeLog()
{
#ifdef USE_NETWORK
	if (!FileHelper::Instance()->IsFileExist("VERSION", true))
	{
		CheckLastVersion(true);
	}
	else
	{
		const auto version = FileHelper::Instance()->LoadFileToString("VERSION", true);
		NetCodeRetriever::Instance()->ParseVersionString(version);
	}
	puShowNewVersionDialog = true;
	puShowChangelog = true;
#endif
}

void VersionSystem::VersionRetrieved()
{
	puShowNewVersionDialog = true;
}

bool VersionSystem::DrawNewVersionDialog()
{
	auto res = false;

#ifdef USE_NETWORK
	if (puShowNewVersionDialog) {
        const auto currentVersion = ct::toStr("%i%i%i", NoodlesPlate_MajorNumber, NoodlesPlate_MinorNumber, NoodlesPlate_BuildNumber);
		const size_t currentVersionID = ct::uvariant(currentVersion).GetU();

		auto newVersion = NetCodeRetriever::sVersion.number;
		ct::replaceString(newVersion, ".", "");
		const size_t newVersionID = ct::uvariant(newVersion).GetU();

		if (newVersionID > currentVersionID || puShowChangelog)
		{
			if (puShowChangelog)
			{
				ImGui::Begin("Changes Log", &puShowNewVersionDialog);
			}
			else
			{
				ImGui::Begin("New version Available", &puShowNewVersionDialog);
			}

			ImGui::Text("Your Version is : %s", NoodlesPlate_BuildId);

			if (!puShowChangelog)
			{
				ImGui::Text("A new Version is Available :");

				ImGui::SameLine();

				ImGui::ClickableTextUrl(NetCodeRetriever::sVersion.number.c_str(), NetCodeRetriever::sVersion.url.c_str());
			}

			ImGui::Separator();

			ImGui::Text("Change Log :\n");

			ImGui::Indent();

			MainFrame_Markdown(NetCodeRetriever::sVersion.changelog);

			ImGui::Unindent();

			ImGui::Separator();

			if (ImGui::ContrastedButton("Ok"))
			{
				puShowNewVersionDialog = false;
				puShowChangelog = false;
				res = true;
			}

			ImGui::End();
		}
		else if (!puCheckVersionAtStart)
		{
			ImGui::Begin("No New Version Available", &puShowNewVersionDialog);

			ImGui::Text("Your Version is the latest !");

			ImGui::Separator();

			if (ImGui::ContrastedButton("Ok"))
			{
				puShowNewVersionDialog = false;
				puShowChangelog = false;
				res = true;
			}

			ImGui::End();
		}
		else
		{
			puShowNewVersionDialog = false;
		}
	}
#endif // #ifdef USE_NETWORK

	return res;
}

std::string VersionSystem::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + "<VersionSystem>\n";
	const auto offset = vOffset + vOffset;

	str += offset + "<checkversionatstart>" + ct::toStr(puCheckVersionAtStartUp ? "true" : "false") + "</checkversionatstart>\n";

	str += vOffset + "</VersionSystem>\n";
	
	return str;
}

bool VersionSystem::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

	if (strName == "VersionSystem")
	{
		for (auto child = vElem->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			RecursParsingConfig(child->ToElement(), vElem);
		}
	}

	if (strParentName == "VersionSystem")
	{
		if (strName == "checkversionatstart")
			puCheckVersionAtStartUp = ct::fvariant(strValue).GetB();
	}

	return false;
}