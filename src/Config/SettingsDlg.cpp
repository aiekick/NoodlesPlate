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

#include "SettingsDlg.h"
#include <Headers/Globals.h>
#include <ImGuiPack.h>
#include <Gui/MainFrame.h>
#include <Backends/MainBackend.h>
#include <SoGLSL/src/Gui/CustomGuiWidgets.h>
#include <Config/StaticConfig.h>
#include <ctools/FileHelper.h>
#include <SoGLSL/src/Importer/NetCodeRetriever.h>
#include <SoGLSL/src/Systems/GamePadSystem.h>
#include <SoGLSL/src/Systems/MidiSystem.h>
#include <SoGLSL/src/Systems/SoundSystem.h>
#include <Systems/PathSystem.h>

///////////////////////////////////////////////////////
//// SettingsDlg //////////////////////////////////////
///////////////////////////////////////////////////////

// cette classe ne doit rien modifier tant que le use n'a pas appuye sur ok
// donc ca ne doit rien modifier dans l'app tant que c'est pas ok
// si cancel ca doit tout annuler
// donc on ne manipule aucune variables externe dans que pas ok

SettingsDlg::SettingsDlg()
{
	puShowDialog = false;
}

SettingsDlg::~SettingsDlg() = default;

void SettingsDlg::OpenDialog()
{
	if (puShowDialog)
		return;

	Init();

	Load();

	puShowDialog = true;
}

void SettingsDlg::CloseDialog()
{
	puShowDialog = false;
}

bool SettingsDlg::DrawDialog()
{
	if (puShowDialog)
	{
		const auto res = false;

		ImGui::Begin("Settings");

		ImGui::Separator();

		DrawCategoryPanes();
		
		ImGui::SameLine();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();

		DrawContentPane();

		ImGui::Separator();

		DrawButtonsPane();

		ImGui::End();

		return res;
	}

	return false;
}

void SettingsDlg::DrawCategoryPanes()
{
	const auto size = ImGui::GetContentRegionMax() - ImVec2(100, 68);

	ImGui::BeginChild("Categories", ImVec2(100, size.y));
		
	for (auto it = puCategories.begin(); it != puCategories.end(); ++it)
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			if (ImGui::Selectable(it2->first.c_str(), puCurrentCategory == it2->first))
			{
				puCurrentCategory = it2->first;
			}
		}
	}
	
	ImGui::EndChild();
}

void SettingsDlg::DrawContentPane()
{
	auto size = ImGui::GetContentRegionMax() - ImVec2(100, 68);

	if (!ImGui::GetCurrentWindow()->ScrollbarY)
	{
		size.x -= ImGui::GetStyle().ScrollbarSize;
	}

	ImGui::BeginChild("##Content", size);

	for (auto it = puCategories.begin(); it != puCategories.end(); ++it)
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			if (puCurrentCategory == it2->first)
			{
				it2->second(SettingsPaneModeEnum::SETTINGS_PANE_MODE_CONTENT);
			}
		}
	}

	ImGui::EndChild();
}

void SettingsDlg::DrawButtonsPane()
{
	if (ImGui::ContrastedButton("Cancel"))
	{
		CloseDialog();
	}

	ImGui::SameLine();
	
	if (ImGui::ContrastedButton("Ok"))
	{
		Save();
		CloseDialog();
	}
}

void SettingsDlg::Load()
{
	for (auto it = puCategories.begin(); it != puCategories.end(); ++it)
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			it2->second(SettingsPaneModeEnum::SETTINGS_PANE_MODE_LOAD);
		}
	}
}

void SettingsDlg::Save()
{
	for (auto it = puCategories.begin(); it != puCategories.end(); ++it)
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			it2->second(SettingsPaneModeEnum::SETTINGS_PANE_MODE_SAVE);
		}
	}

	MainFrame::Instance()->SaveConfigFile("config.xml");
}

/////////////////////////////////////////////////////////////////////////

void SettingsDlg::Init()
{
	puCategories[0]["General"] = std::bind(&SettingsDlg::DrawPane_General, this, std::placeholders::_1);
	puCategories[1]["Paths"] = std::bind(&SettingsDlg::DrawPane_Paths, this, std::placeholders::_1);
	puCategories[2]["Style"] = std::bind(&SettingsDlg::DrawPane_Style, this, std::placeholders::_1);
	puCategories[3]["Internet"] = std::bind(&SettingsDlg::DrawPane_Internet, this, std::placeholders::_1);
	puCategories[4]["Gamepad"] = std::bind(&SettingsDlg::DrawPane_Gamepad, this, std::placeholders::_1);
	puCategories[5]["Midi"] = std::bind(&SettingsDlg::DrawPane_Midi, this, std::placeholders::_1);
	//puCategories[6]["Sound"] = std::bind(&SettingsDlg::DrawPane_Sound, this, std::placeholders::_1);

	puCurrentCategory = "General";
}

void SettingsDlg::DrawPane_General(const SettingsPaneModeEnum& vMode)
{
	if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_LOAD)
	{
		
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_SAVE)
	{
		
	}
	else if(vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_CONTENT)
	{
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Activate/Deactivate App Background Transparancy\n(use the current FrameBuffer Alpha Value)\n Need a restart of NoodlesPlate");
		ImGui::CheckBoxBoolDefault("App Background Transparancy", &StaticConfig::Instance()->puUseTransparancy, false);

		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Activate/Deactivate the HDPI Support. Need a restart of NoodlesPlate");
		ImGui::CheckBoxBoolDefault("HDPI Support", &StaticConfig::Instance()->puEnableDPISupport, false);

		auto& io = ImGui::GetIO(); (void)io;
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Global UI Scale");
		if (ImGui::SliderFloatDefaultCompact(ImGui::GetContentRegionAvail().x - 50.0f, "Global UI Scale", &StaticConfig::Instance()->puGlobalUIScale, 0.1f, 10.0f, 1.0f, 0.1f))
		{
			io.FontGlobalScale = StaticConfig::Instance()->puGlobalUIScale;
		}
	}
}

void SettingsDlg::DrawPane_Paths(const SettingsPaneModeEnum& vMode)
{
	if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_LOAD)
	{
		PathSystem::Instance()->Load();
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_SAVE)
	{
		PathSystem::Instance()->Save();
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_CONTENT)
	{
		PathSystem::Instance()->Draw();
	}
}

void SettingsDlg::DrawPane_Internet(const SettingsPaneModeEnum& vMode)
{
#ifdef USE_NETWORK
	static bool _ProxyUse = false;
	static char _ProxyIp[100] = "ip";
	static char _ProxyPort[100] = "port";
	static char _ProxyUser[100] = "user";
	static char _ProxyPwd[100] = "password";
#ifdef _DEBUG
	static char _ShaderToyApiKey[100] = "ftnKwW";
#else
	static char _ShaderToyApiKey[100] = "Api Key";
#endif
	static bool _ChecLastVersionAtStart = false;

	// ShaderToy api key code
	// proxy seetings
	// proxy, ip + port
	// proxy user + pass
	// check version at start

	if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_LOAD)
	{
		const auto proxyPath = NetCodeRetriever::Instance()->GetProxyPath();
		if (!proxyPath.empty())
		{
			auto vec = ct::splitStringToVector(proxyPath, ":");
			if (vec.size() > 0)
			{
#ifdef MSVC
				strncpy_s(_ProxyIp, vec[0].c_str(), ct::mini<size_t>(vec[0].size(), 100));
#else
				strncpy(_ProxyIp, vec[0].c_str(), ct::mini<size_t>(vec[0].size(), 100));
#endif
				_ProxyIp[vec[0].size()] = '\0';
			}
			if (vec.size() > 1)
			{
#ifdef MSVC
				strncpy_s(_ProxyPort, vec[1].c_str(), ct::mini<size_t>(vec[1].size(), 100));
#else
				strncpy(_ProxyPort, vec[1].c_str(), ct::mini<size_t>(vec[1].size(), 100));
#endif
				_ProxyPort[vec[1].size()] = '\0';
			}
		}

		const auto proxyUserPwd = NetCodeRetriever::Instance()->GetProxyUserPwd();
		if (!proxyUserPwd.empty())
		{
			auto vec = ct::splitStringToVector(proxyUserPwd, ":");
			if (vec.size() > 0)
			{
#ifdef MSVC
				strncpy_s(_ProxyUser, vec[0].c_str(), ct::mini<size_t>(vec[0].size(), 100));
#else
				strncpy(_ProxyUser, vec[0].c_str(), ct::mini<size_t>(vec[0].size(), 100));
#endif
				_ProxyUser[vec[0].size()] = '\0';
			}
			if (vec.size() > 1)
			{
#ifdef MSVC
				strncpy_s(_ProxyPwd, vec[1].c_str(), ct::mini<size_t>(vec[1].size(), 100));
#else
				strncpy(_ProxyPwd, vec[1].c_str(), ct::mini<size_t>(vec[1].size(), 100));
#endif
				_ProxyPwd[vec[1].size()] = '\0';
			}
		}

		const auto shaApiKey = NetCodeRetriever::Instance()->GetShaderToyApiKey();
		if (!shaApiKey.empty())
		{
#ifdef MSVC
			strncpy_s(_ShaderToyApiKey, shaApiKey.c_str(), ct::mini<size_t>(shaApiKey.size(), 100));
#else
			strncpy(_ShaderToyApiKey, shaApiKey.c_str(), ct::mini<size_t>(shaApiKey.size(), 100));
#endif
			_ShaderToyApiKey[shaApiKey.size()] = '\0';
		}

		_ProxyUse = NetCodeRetriever::Instance()->GetProxyUse();
		_ChecLastVersionAtStart = NetCodeRetriever::Instance()->puCheckVersionAtStart;

	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_SAVE)
	{
		NetCodeRetriever::Instance()->SetProxyPath(std::string(_ProxyIp) + ":" + std::string(_ProxyPort));
		NetCodeRetriever::Instance()->SetProxyUserPwd(std::string(_ProxyUser) + ":" + std::string(_ProxyPwd));
		NetCodeRetriever::Instance()->SetProxyUse(_ProxyUse);
		NetCodeRetriever::Instance()->SetShaderToyApiKey(std::string(_ShaderToyApiKey));
		NetCodeRetriever::Instance()->puCheckVersionAtStart = _ChecLastVersionAtStart;
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_CONTENT)
	{
		ImGui::Text("Proxy :");
		
		ImGui::Text("Use Proxy ?");
		ImGui::SameLine(100);
		ImGui::Checkbox("##UseProxy?", &_ProxyUse);

		if (_ProxyUse)
		{
			ImGui::Indent();

			ImGui::Text("Ip");
			ImGui::SameLine(100);
			ImGui::PushItemWidth(200);
			ImGui::InputText("##ip", _ProxyIp, 100);
			ImGui::PopItemWidth();

			ImGui::Text("Port");
			ImGui::SameLine(100);
			ImGui::PushItemWidth(200);
			ImGui::InputText("##port", _ProxyPort, 100);
			ImGui::PopItemWidth();

			ImGui::Text("User");
			ImGui::SameLine(100);
			ImGui::PushItemWidth(200);
			ImGui::InputText("##user", _ProxyUser, 100);
			ImGui::PopItemWidth();

			ImGui::Text("Password");
			ImGui::SameLine(100);
			ImGui::PushItemWidth(200);
			ImGui::InputText("##password", _ProxyPwd, 100);
			ImGui::PopItemWidth();

			ImGui::Unindent();
		}
		
		ImGui::Separator();

		ImGui::Text("ShaderToy Api Key :");
		ImGui::Indent();
		ImGui::PushItemWidth(200);
		ImGui::InputText("##shadertoyapikey", _ShaderToyApiKey, 10);
		ImGui::PopItemWidth();
		ImGui::Unindent();

		ImGui::Separator();

		ImGui::Text("Version :");

		ImGui::Checkbox("Check iF you have the Last Version At Start ?", &_ChecLastVersionAtStart);
	}
#else
	UNUSED(vMode);
#endif // #ifdef USE_NETWORK
}

void SettingsDlg::DrawPane_Library(const SettingsPaneModeEnum& vMode)
{
	// reset lib
	// add / remove links
	// rename

	if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_LOAD)
	{

	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_SAVE)
	{

	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_CONTENT)
	{

	}
}

void SettingsDlg::DrawPane_FontConfig(const SettingsPaneModeEnum& vMode) //-V524
{
	if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_LOAD)
	{

	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_SAVE)
	{

	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_CONTENT)
	{

	}
}

void SettingsDlg::DrawPane_Gamepad(const SettingsPaneModeEnum& vMode)
{
	if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_LOAD)
	{
		GamePadSystem::Instance()->LoadSettings();
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_SAVE)
	{
		GamePadSystem::Instance()->SaveSettings();
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_CONTENT)
	{
		GamePadSystem::Instance()->ShowContent();
	}
}

void SettingsDlg::DrawPane_Midi(const SettingsPaneModeEnum& vMode)
{
	if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_LOAD)
	{
		MidiSystem::Instance()->LoadSettings();
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_SAVE)
	{
		MidiSystem::Instance()->SaveSettings();
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_CONTENT)
	{
		MidiSystem::Instance()->ShowContent(MainBackend::sMainThread);
	}
}

void SettingsDlg::DrawPane_Sound(const SettingsPaneModeEnum& vMode)
{
	if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_LOAD)
	{
		//SoundSystem::Instance()->LoadSettings();
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_SAVE)
	{
		//SoundSystem::Instance()->SaveSettings();
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_CONTENT)
	{
		//SoundSystem::Instance()->ShowContent(MainBackend::sMainThread);
	}
}

void SettingsDlg::DrawPane_Style(const SettingsPaneModeEnum& vMode)
{
	if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_LOAD)
	{
		
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_SAVE)
	{
		
	}
	else if (vMode == SettingsPaneModeEnum::SETTINGS_PANE_MODE_CONTENT)
	{
        ImGuiThemeHelper::Instance()->DrawFileStyle();
	}
}

std::string SettingsDlg::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + "<Settings>\n";


	str += vOffset + "</Settings>\n";

	return str;
}

bool SettingsDlg::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

	if (strParentName == "Settings")
	{

	}

	return false;
}