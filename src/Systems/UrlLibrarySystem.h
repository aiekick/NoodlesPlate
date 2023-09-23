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

#include <ctools/cTools.h>
#include <ctools/ConfigAbstract.h>

#include <map>

class UrlLibrarySystem : public conf::ConfigAbstract
{
private:
	std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> puSampleUrls;
	std::string puUrlLibrarySystemPlatform;
	std::string puUrlLibrarySystemUser;
	std::string puUrlLibrarySystemDesc;
	std::string puUrlLibrarySystemUrl;

public:
	static UrlLibrarySystem* Instance()
	{
		static UrlLibrarySystem _instance;
		return &_instance;
	}

protected:
	UrlLibrarySystem(); // Prevent construction
	UrlLibrarySystem(const UrlLibrarySystem&) {}; // Prevent construction by copying
	UrlLibrarySystem& operator =(const UrlLibrarySystem&) { return *this; }; // Prevent assignment
	~UrlLibrarySystem(); // Prevent unwanted destruction
	
public:
	void Init();
	void DrawImGui_Library();
	void AddUrl(std::string vPlatform, std::string vUser, const std::string& vName, std::string vUrl);

public:
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;

};
