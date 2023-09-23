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

#include <ctools/ConfigAbstract.h>

/*
on doit utiliser cette classe dans les cas :
- quand on veut charger des parametres au plus tot dans le soft
- quand on veut avoir des parametres globaux li� au soft et non a une classe
*/

class StaticConfig : public conf::ConfigAbstract
{
public: // params
	bool puUseTransparancy = false;
	float puGlobalUIScale = 1.0f;
	bool puEnableDPISupport = false;

public:
	static StaticConfig* Instance()
	{
		static StaticConfig _instance;
		return &_instance;
	}

protected:
	StaticConfig(); // Prevent construction
	StaticConfig(const StaticConfig&) {}; // Prevent construction by copying
	StaticConfig& operator =(const StaticConfig&) { return *this; }; // Prevent assignment
	~StaticConfig() = default; // Prevent unwanted destruction

public:
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;
};
