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

// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ProjectFile.h"

#include <Messaging.h>
#include <ctools/FileHelper.h>

ProjectFile::ProjectFile() = default;


ProjectFile::~ProjectFile() = default;

void ProjectFile::Clear()
{
	puProjectFilePathName.clear();
	puProjectFilePath.clear();
	puIsLoaded = false;
	puIsThereAnyNotSavedChanged = false;
	
	Messaging::Instance()->Clear();
}

void ProjectFile::New()
{
	Clear();
	puIsLoaded = true;
	puNeverSaved = true;
}

void ProjectFile::New(const std::string& vFilePathName)
{
	Clear();
	puProjectFilePathName = FileHelper::Instance()->SimplifyFilePath(vFilePathName);
	const auto ps = FileHelper::Instance()->ParsePathFileName(puProjectFilePathName);
	if (ps.isOk)
	{
		puProjectFilePath = ps.path;
	}
	puIsLoaded = true;
	SetProjectChange(false);
}

/*void ProjectFile::LoadFile(const std::string& vFilePathName)
{
	puProjectFilePathName = FileHelper::Instance()->SimplifyFilePath(vFilePathName);
	const auto ps = FileHelper::Instance()->ParsePathFileName(puProjectFilePathName);
	if (ps.isOk)
	{
		puProjectFilePath = ps.path;
	}
}*/

bool ProjectFile::Load()
{
	return LoadAs(puProjectFilePathName);
}

// ils wanted to not pass the adress for re open case
// elwse, the clear will set vFilePathName to empty because with re open, target puProjectFilePathName
bool ProjectFile::LoadAs(const std::string vFilePathName)  
{
	Clear();
	const auto filePathName = FileHelper::Instance()->SimplifyFilePath(vFilePathName);
	if (LoadConfigFile(filePathName))
	{
		puProjectFilePathName = filePathName;
		const auto ps = FileHelper::Instance()->ParsePathFileName(puProjectFilePathName);
		if (ps.isOk)
		{
			puProjectFilePath = ps.path;
		}
		puIsLoaded = true;
		SetProjectChange(false);
	}
	else
	{
		Clear();
	}

	return puIsLoaded;
}

bool ProjectFile::Save()
{
	if (puNeverSaved) 
		return false;

	if (SaveConfigFile(puProjectFilePathName))
	{
		SetProjectChange(false);
		return true;
	}

	return false;
}

bool ProjectFile::SaveAs(const std::string& vFilePathName)
{
	const auto filePathName = FileHelper::Instance()->SimplifyFilePath(vFilePathName);
	const auto ps = FileHelper::Instance()->ParsePathFileName(filePathName);
	if (ps.isOk)
	{
		puProjectFilePathName = FileHelper::Instance()->ComposePath(ps.path, ps.name, "ifs");
		puProjectFilePath = ps.path;
		puNeverSaved = false;
		return Save();
	}
	return false;
}

bool ProjectFile::IsLoaded() const
{
	return puIsLoaded;
}

bool ProjectFile::IsThereAnyNotSavedChanged() const
{
	return puIsThereAnyNotSavedChanged;
}

void ProjectFile::SetProjectChange(bool vChange)
{
	puIsThereAnyNotSavedChanged = vChange;
}

std::string ProjectFile::GetAbsolutePath(const std::string& vFilePathName) const
{
	auto res = vFilePathName;

	if (!vFilePathName.empty())
	{
		if (!FileHelper::Instance()->IsAbsolutePath(vFilePathName)) // relative
		{
			res = FileHelper::Instance()->SimplifyFilePath(
				puProjectFilePath + FileHelper::Instance()->puSlashType + vFilePathName);
		}
	}

	return res;
}

std::string ProjectFile::GetRelativePath(const std::string& vFilePathName) const
{
	auto res = vFilePathName;

	if (!vFilePathName.empty())
	{
		res = FileHelper::Instance()->GetRelativePathToPath(vFilePathName, puProjectFilePath);
	}

	return res;
}

std::string ProjectFile::getXml(const std::string& vOffset, const std::string& /*vUserDatas*/) {
	std::string str;

	str += vOffset + "<project>\n";

	str += vOffset + "</project>\n";

	return str;
}

bool ProjectFile::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& /*vUserDatas*/) {
	// The value of this child identifies the name of this element
	std::string strName;
	std::string strValue;
	std::string strParentName;

	strName = vElem->Value();
	if (vElem->GetText())
		strValue = vElem->GetText();
	if (vParent != nullptr)
		strParentName = vParent->Value();

	if (strName == "project")
	{

	}

	if (strParentName == "project")
	{
		
	}

	return true;
}

