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

#include <AbstractPane.h>
#include <ctools/ConfigAbstract.h>
#include <ImGuiFileDialog.h>

#include <string>

class ProjectFile;
class ProfilerPane : public AbstractPane, public conf::ConfigAbstract
{
private:
    PaneFlags m_InOutPaneShown = -1;

public:
	bool Init() override;
	void Unit() override;
	bool DrawPanes(const uint32_t& vCurrentFrame, PaneFlags& vInOutPaneShown, ImGuiContext* vContextPtr = nullptr, const std::string& vUserDatas = {}) override;
    
	// configuration
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
	
public: // singleton
	static std::shared_ptr<ProfilerPane> Instance()
	{
		static auto _instance = std::make_shared<ProfilerPane>();
		return _instance;
	}

public:
	ProfilerPane(); // Prevent construction
	ProfilerPane(const ProfilerPane&) {}; // Prevent construction by copying
	ProfilerPane& operator =(const ProfilerPane&) { return *this; }; // Prevent assignment
	~ProfilerPane(); // Prevent unwanted destruction};
};

