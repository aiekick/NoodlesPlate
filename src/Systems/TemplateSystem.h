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

#include <ImGuiPack.h>
#include <Headers/RenderPackHeaders.h>

#include <string>
#include <map>

struct TemplateFile
{
	std::string path;
	std::string main;
	std::map<std::string, std::string> buffers;
	std::map<std::string, std::string> includes;
	std::map<std::string, bool> errors;
	std::map<std::string, std::string> newNames;

	TemplateFile()
	{
		
	}
	void AddBuffer(std::string vid, std::string vBuffer);
	void AddInclude(std::string vid, std::string vInclude);
};

struct TemplateCategorie
{
	std::string directory;
	std::string fullPath;
	std::map<std::string, TemplateCategorie> categories;
	std::map< std::string, TemplateFile> fileNames;
	
	TemplateCategorie();
	bool DrawMenu();
	void Clear();
	bool HasChilds();
};

class ShaderKey;
class RenderPack;
class TemplateSystem
{
private:
	TemplateFile puSelectedFileToCopy;
	TemplateCategorie puRootCategorie;
	
public:
	static TemplateSystem* Instance()
	{
		static TemplateSystem _instance;
		return &_instance;
	}

protected:
	TemplateSystem(); // Prevent construction
	TemplateSystem(const TemplateSystem&) {}; // Prevent construction by copying
	TemplateSystem& operator =(const TemplateSystem&) { return *this; }; // Prevent assignment
	~TemplateSystem(); // Prevent unwanted destruction

public:
	bool Init();

	bool DrawMenu();

	void CopyFromTemplate(TemplateFile vTemplateFile);
	bool CreateFileFromSelectedTemplate(const std::string& vFileName, const std::string& vFilePath, std::string *vMainFilepathName);
	void CreateTemplateFromRenderPack(const std::string& vMainFileName, const std::string& vPath, RenderPackWeak vMainRenderPack);
	
private:
	void SetNameOfBuffers(const char* vFilter, IGFDUserDatas vUserDatas, bool* vCantContinue);

	void CreateTemplate(const std::string& vFileName, const std::string& vFilePath, std::map<std::string, ShaderKeyPtr> vKeys);
	void ScanTemplateDir(const std::string& vPath);
	void ScanTemplateDirRecurs(TemplateCategorie *vCategorieToExplore);
};
