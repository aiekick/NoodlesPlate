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
class TimeLinePane : public AbstractPane, public conf::ConfigAbstract
{
public:
	bool Init() override;
	void Unit() override;
	bool DrawPanes(const uint32_t& vCurrentFrame, PaneFlags& vInOutPaneShown, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    
	// configuration
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
	
public: // singleton
	static std::shared_ptr<TimeLinePane> Instance()
	{
		static auto _instance = std::make_shared<TimeLinePane>();
		return _instance;
	}

public:
	TimeLinePane(); // Prevent construction
	TimeLinePane(const TimeLinePane&) {}; // Prevent construction by copying
	TimeLinePane& operator =(const TimeLinePane&) { return *this; }; // Prevent assignment
	~TimeLinePane(); // Prevent unwanted destruction};
};

