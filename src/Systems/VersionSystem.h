// NoodlesPlate Copyright (C) 2017-2024 Stephane Cuillerdier aka Aiekick
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


class VersionSystem : public conf::ConfigAbstract
{
public:
	bool puCheckVersionAtStartUp = true;
	bool puShowNewVersionDialog = false;
	bool puShowChangelog = false;
	bool puNeedCheckVersionAtStart = false;
	bool puCheckVersionAtStart = false;

public:
	static VersionSystem* Instance()
	{
		static VersionSystem _instance;
		return &_instance;
	}

protected:
	VersionSystem(); // Prevent construction
	VersionSystem(const VersionSystem&) {}; // Prevent construction by copying
	VersionSystem& operator =(const VersionSystem&) { return *this; }; // Prevent assignment
	~VersionSystem(); // Prevent unwanted destruction
	
public:
	void ShowVersionChangeLog();
	void VersionRetrieved();
	bool DrawNewVersionDialog();
	void CheckLastVersion(bool vForce = false);
	void CheckVersionAtStart();

public:
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;
};

