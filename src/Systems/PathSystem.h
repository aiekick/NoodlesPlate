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

class PathField : public conf::ConfigAbstract
{
private:
	char m_New_Path_Buffer[2047 + 1] = "";
	std::string m_Original_Use; // give the use for load save xml
	std::string m_Original_Path;

public:
	void SetUse(const std::string& vUse);
	void SetPath(const std::string& vFilePathName);
	std::string GetPath();
	void Draw(const char* vButtonLabel);
	void Apply(); // apply change
public:
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "")override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
};

class PathSystem : public conf::ConfigAbstract
{
private:
	PathField m_PathField_Scripts;
	PathField m_PathField_Assets_General;
	PathField m_PathField_Assets_Texture_2D;
	PathField m_PathField_Assets_Texture_3D;
	PathField m_PathField_Assets_CubeMap;
#ifdef USE_SOUNDS
	PathField m_PathField_Assets_Sounds;
#endif
	PathField m_PathField_Assets_Meshs;
	PathField m_PathField_Assets_Meshs_Export;

public:
	void Load();
	void Save();
	void Draw();

public:
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;

public:
	static PathSystem* Instance()
	{
		static PathSystem _instance;
		return &_instance;
	}

protected:
	PathSystem(); // Prevent construction
	PathSystem(const PathSystem&) {}; // Prevent construction by copying
	PathSystem& operator =(const PathSystem&) { return *this; }; // Prevent assignment
	~PathSystem(); // Prevent unwanted destruction
};
