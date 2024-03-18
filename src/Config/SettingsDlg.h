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

#pragma once

#include <vector>
#include <map>
#include <string>

#include <future>
#include <functional>
#include <string>
#include <list>

#include <tinyxml2.h>

#include <ctools/ConfigAbstract.h>

enum class SettingsPaneModeEnum
{
	SETTINGS_PANE_MODE_CONTENT = 0,
	SETTINGS_PANE_MODE_LOAD,
	SETTINGS_PANE_MODE_SAVE,
	SETTINGS_PANE_MODE_Count
};

class SettingsDlg : public conf::ConfigAbstract
{
private:
	bool puShowDialog = false;
	std::string dlg_key;
	std::map<int, std::map<std::string, std::function<void(SettingsPaneModeEnum)>>> puCategories;
	std::string puCurrentCategory;

public:
	static SettingsDlg* Instance()
	{
		static SettingsDlg _instance;
		return &_instance;
	}

protected:
	SettingsDlg(); // Prevent construction
	SettingsDlg(const SettingsDlg&) {}; // Prevent construction by copying
	SettingsDlg& operator =(const SettingsDlg&) { return *this; }; // Prevent assignment
	~SettingsDlg(); // Prevent unwanted destruction

public:
	void Init();

	void OpenDialog();
	void CloseDialog();
	bool DrawDialog();

private:
	void DrawCategoryPanes();
	void DrawContentPane();
	void DrawButtonsPane();

private:
	void Load();
	void Save();

private:
	void DrawPane_General(const SettingsPaneModeEnum& vMode);
	void DrawPane_Paths(const SettingsPaneModeEnum& vMode);
	void DrawPane_Internet(const SettingsPaneModeEnum& vMode);
	void DrawPane_Library(const SettingsPaneModeEnum& vMode);
	void DrawPane_FontConfig(const SettingsPaneModeEnum& vMode);
	void DrawPane_Gamepad(const SettingsPaneModeEnum& vMode);
	void DrawPane_Midi(const SettingsPaneModeEnum& vMode);
	void DrawPane_Sound(const SettingsPaneModeEnum& vMode);
	void DrawPane_Style(const SettingsPaneModeEnum& vMode);

public:
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "")override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
};
