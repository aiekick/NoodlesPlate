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


#include "TemplateSystem.h"

#include <filesystem>
#include <ctools/FileHelper.h>
#include <ctools/Logger.h>
#include <ImGuiPack.h>

#include <SoGLSL/src/CodeTree/CodeTree.h>
#include <SoGLSL/src/CodeTree/ShaderKey.h>
#include <SoGLSL/src/Gui/CustomGuiWidgets.h>
#include <SoGLSL/src/Renderer/RenderPack.h>
#include <SoGLSL/src/Uniforms/UniformVariant.h>

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

void TemplateFile::AddBuffer(std::string vid, std::string vBuffer)
{
	buffers[vid] = vBuffer;
	newNames[vid] = vBuffer;
	errors[vid] = true;
}

void TemplateFile::AddInclude(std::string vid, std::string vInclude)
{
	includes[vid] = vInclude;
	newNames[vid] = vInclude;
	errors[vid] = true;
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

TemplateCategorie::TemplateCategorie()
{

}

bool TemplateCategorie::DrawMenu()
{
	const auto res = false;

	for (auto it = categories.begin(); it != categories.end(); ++it)
	{
		if (ImGui::BeginMenu(it->first.c_str()))
		{
			it->second.DrawMenu();

			ImGui::EndMenu();
		}
	}

	for (auto it = fileNames.begin(); it != fileNames.end(); ++it)
	{
		if (ImGui::MenuItem(it->first.c_str()))
		{
			TemplateSystem::Instance()->CopyFromTemplate(it->second);
		}
	}

	return res;
}

void TemplateCategorie::Clear()
{
	directory = "";
	fullPath = "";
	categories.clear();
	fileNames.clear();
}

bool TemplateCategorie::HasChilds()
{
	return (!categories.empty() || !fileNames.empty());
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

TemplateSystem::TemplateSystem()
{

}

TemplateSystem::~TemplateSystem()
{

}

bool TemplateSystem::Init()
{
	// on cree le repertoire qui va contenir les templates
	FileHelper::Instance()->CreateDirectoryIfNotExist("templates");
	
	// on va scanner ce repertoire, avec un niveau de recursion permis
	// les sous repertoire vont devenir la 1ere categorie
	ScanTemplateDir("templates");

	return true;
}

void TemplateSystem::ScanTemplateDir(const std::string& vPath)
{
	puRootCategorie.Clear();
	puRootCategorie.directory = vPath;
	puRootCategorie.fullPath = vPath;

#ifdef SHOW_LOGS_FOR_TEMPLATES
	LogVarDebug("--- Start Scan for Templates --");
#endif

	ScanTemplateDirRecurs(&puRootCategorie);

#ifdef SHOW_LOGS_FOR_TEMPLATES
	LogVarDebug("--- End Scan for Templates ----");
#endif
}

void TemplateSystem::ScanTemplateDirRecurs(TemplateCategorie *vCategorieToExplore)
{
	if (!vCategorieToExplore) return;

	std::filesystem::path path = vCategorieToExplore->fullPath;

	if (!std::filesystem::is_directory(path))
	{
		path = "."; // current  app path
		if (!std::filesystem::is_directory(path))
		{
			return;
		}
	}
	
	std::string fileName;
	
	/* Scan files in directory */
	for (auto const& dir_entry : std::filesystem::directory_iterator{ path })
	{
		auto _path = dir_entry.path();
		fileName = _path.filename().string();

		if (fileName != "." && fileName != "..")
		{
			if (dir_entry.is_directory()) // dir => recurs
			{
				auto cat = &vCategorieToExplore->categories[fileName];

				cat->directory = fileName;

				auto newPath = _path.string();

				cat->fullPath = newPath;

				ScanTemplateDirRecurs(cat);
			}
			else if (dir_entry.is_regular_file()) // file => list
			{
				auto fullPathFileName = _path.string();

				auto p = fileName.find(".main");
				if (p != std::string::npos)
				{
					auto name = fileName.substr(0, p);
					auto ext = fileName.substr(p + 1);

					auto tfile = vCategorieToExplore->fileNames[name];
					tfile.path = _path.parent_path().string();
					tfile.main = name;

					vCategorieToExplore->fileNames[name] = tfile;

#ifdef SHOW_LOGS_FOR_TEMPLATES
					LogVarDebug("Main Template Found : ");
#endif
				}

				p = fileName.find(".buffer");
				if (p != std::string::npos)
				{
					auto name = fileName.substr(0, p);
					auto ext = fileName.substr(p + 1);

					auto tfile = vCategorieToExplore->fileNames[name];
					tfile.AddBuffer(ext, fileName);

					vCategorieToExplore->fileNames[name] = tfile;

#ifdef SHOW_LOGS_FOR_TEMPLATES
					LogVarDebug("Buffer Template Found : ");
#endif
				}

				p = fileName.find(".include");
				if (p != std::string::npos)
				{
					auto name = fileName.substr(0, p);
					auto ext = fileName.substr(p + 1);

					auto tfile = vCategorieToExplore->fileNames[name];
					tfile.AddInclude(ext, fileName);

					vCategorieToExplore->fileNames[name] = tfile;

#ifdef SHOW_LOGS_FOR_TEMPLATES
					LogVarDebug("Include Template Found : ");
#endif
				}
			}
		}
	}
}

bool TemplateSystem::DrawMenu()
{
	auto res = false;

	if (puRootCategorie.HasChilds())
	{
		if (ImGui::BeginMenu("Templates"))
		{
			res = puRootCategorie.DrawMenu();

			ImGui::EndMenu();
		}

		ImGui::Separator();
	}
	
	return res;
}

void TemplateSystem::CopyFromTemplate(TemplateFile vTemplateFile)
{
	puSelectedFileToCopy = vTemplateFile;
	ImGuiFileDialog::Instance()->OpenDialogWithPane("NewFileDialog", "New File from template", ".glsl",
		ImGuiFileDialog::Instance()->GetCurrentPath(), puSelectedFileToCopy.main,
		std::bind(&TemplateSystem::SetNameOfBuffers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 
		350, 1, IGFDUserDatas("createfromtemplate"), ImGuiFileDialogFlags_DisableThumbnailMode | ImGuiFileDialogFlags_Modal);
}

void TemplateSystem::SetNameOfBuffers(const char * /*vFilter*/, IGFDUserDatas /*vUserDatas*/, bool *vCantContinue)
{
	ImGui::TextColored(ImVec4(0, 1, 1, 1), "Choose New name for child buffers :");
	char buffer[100] = "";

	auto _globalOK = true;

	for (auto it = puSelectedFileToCopy.newNames.begin(); it != puSelectedFileToCopy.newNames.end(); ++it)
	{
		ImGui::Text(it->first.c_str());
		snprintf(buffer, 99, it->second.c_str());
		ImGui::PushID(ImGui::IncPUSHID());
		auto isOk = puSelectedFileToCopy.errors[it->first];
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetUniformLocColor(isOk ? 1 : -1));
		if (ImGui::InputText("", buffer, 99))
		{
			std::string name = buffer;

			isOk = true;

			if (name == puSelectedFileToCopy.main) 
				isOk = false;
			
			for (auto it2 = puSelectedFileToCopy.buffers.begin(); it2 != puSelectedFileToCopy.buffers.end(); ++it2)
			{
				if (it->first != it2->first)
				{
					if (name == it2->first) isOk = false;
					if (name == it2->second) isOk = false;
				}
			}

			for (auto it2 = puSelectedFileToCopy.includes.begin(); it2 != puSelectedFileToCopy.includes.end(); ++it2)
			{
				if (it->first != it2->first)
				{
					if (name == it2->first) isOk = false;
					if (name == it2->second) isOk = false;
				}
			}

			it->second = buffer;
		}

		puSelectedFileToCopy.errors[it->first] = isOk;

		ImGui::PopStyleColor();
		ImGui::PopID();

		_globalOK &= isOk;
	}

	if (vCantContinue)
		*vCantContinue = _globalOK;
}

bool TemplateSystem::CreateFileFromSelectedTemplate(const std::string& vFileName, const std::string& vFilePath, std::string *vMainFilepathName)
{
	auto res = false;

	if (!puSelectedFileToCopy.main.empty() && !vFileName.empty())
	{
		auto fileName = puSelectedFileToCopy.path + FileHelper::Instance()->puSlashType + puSelectedFileToCopy.main;
		auto mainCode = FileHelper::Instance()->LoadFile(fileName + ".main", 0);
		if (!mainCode.empty())
		{
			std::map<std::string, std::string> codes;
			codes["main"] = mainCode;
			std::map<std::string, std::string> newNames;
			auto mainFileName = vFileName;
			ct::replaceString(mainFileName, ".glsl", "");
			newNames["main"] = mainFileName;
			
			std::map<std::string, bool> includes;

			// load code of buffers
			for (auto it = puSelectedFileToCopy.buffers.begin(); it != puSelectedFileToCopy.buffers.end(); ++it)
			{
				fileName = puSelectedFileToCopy.path + FileHelper::Instance()->puSlashType + it->second;
				auto code = FileHelper::Instance()->LoadFile(fileName, 0);
				if (!code.empty())
				{
					codes[it->first] = code;
					includes[it->first] = false;
					newNames[it->first] = puSelectedFileToCopy.newNames[it->first];
				}
				else
				{
					LogVarError("[ERROR] Can't find the Buffer template file %s. Continue, but maybe, the template copy will not work properly.", fileName.c_str());
				}
			}

			// load code of includes
			for (auto it = puSelectedFileToCopy.includes.begin(); it != puSelectedFileToCopy.includes.end(); ++it)
			{
				fileName = puSelectedFileToCopy.path + FileHelper::Instance()->puSlashType + it->second;
				auto code = FileHelper::Instance()->LoadFile(fileName, 0);
				if (!code.empty())
				{
					codes[it->first] = code;
					includes[it->first] = true;
					newNames[it->first] = puSelectedFileToCopy.newNames[it->first];
				}
				else
				{
					LogVarError("[ERROR] Can't find the Buffer template file %s. Continue, but maybe, the template copy will not work properly.", fileName.c_str());
				}
			}

			// replace id in all codes
			std::string key, newkey;
			for (auto itFirstRing = codes.begin(); itFirstRing != codes.end(); ++itFirstRing)
			{
				for (auto itSecondRing = newNames.begin(); itSecondRing != newNames.end(); ++itSecondRing)
				{
					// replace newName->first by newName->second in code
					key = "[[" + itSecondRing->first + "]]";
					
					if (includes[itSecondRing->first])
					{
						newkey = "\"" + itSecondRing->second + ".glsl\"";
					}
					else
					{
						newkey = itSecondRing->second;
					}

					ct::replaceString(itFirstRing->second, key, newkey);
				}
			}

			// save files
			for (auto it = newNames.begin(); it != newNames.end(); ++it)
			{
				auto code = codes[it->first];
				auto path = vFilePath + FileHelper::Instance()->puSlashType + it->second + ".glsl";
				if (!code.empty())
				{
					FileHelper::Instance()->SaveToFile(code, path, 0);

					if (it->first == "main")
					{
						if (vMainFilepathName)
						{
							*vMainFilepathName = path;
						}
					}
				}
				else
				{
					LogVarLightError("[ERROR] Can't create template file for key %s to path", it->first.c_str(), path.c_str());
				}
			}
		}
		else
		{
			LogVarError("[ERROR] Can't find the Buffer template file %s. Continue, but maybe, the template copy will not work properly.", fileName.c_str());
		}

		res = true;
	}

	return res;
}

void TemplateSystem::CreateTemplateFromRenderPack(const std::string& vMainFileName, const std::string& vPath, RenderPackWeak vMainRenderPack)
{
	auto rpPtr = vMainRenderPack.lock();
	if (rpPtr && !vMainFileName.empty())
	{
		std::map<std::string, ShaderKeyPtr> keys;

		auto key = rpPtr->GetShaderKey();
		if (key)
		{
			keys["main"] = key;

			auto idxInclude = 0;
			for (auto it = key->puIncludeFileNames.begin(); it != key->puIncludeFileNames.end(); ++it)
			{
				if (key->puParentCodeTree)
				{
					auto incKey = key->puParentCodeTree->GetIncludeKey(it->first);
					if (incKey)
					{
						auto arr = ct::splitStringToSet(incKey->GetPath(), "\\/");
						if (arr.find("predefined") == arr.end())
							keys["include." + ct::toStr(++idxInclude)] = incKey;
					}
				}
			}

			auto idxBuffer = 0;
			for (auto it : rpPtr->puBuffers)
			{
				auto itPtr = it.lock();
				if (itPtr)
				{
					auto subKey = itPtr->GetShaderKey();
					if (subKey)
					{
						for (auto itInc = subKey->puIncludeFileNames.begin(); itInc != subKey->puIncludeFileNames.end(); ++itInc)
						{
							if (subKey->puParentCodeTree)
							{
								auto incKey = subKey->puParentCodeTree->GetIncludeKey(itPtr->puName);
								if (incKey)
								{
									auto arr = ct::splitStringToSet(incKey->GetPath(), "\\/");
									if (arr.find("predefined") == arr.end())
										keys["include." + ct::toStr(++idxInclude)] = incKey;
								}
							}
						}

						keys["buffer." + ct::toStr(++idxBuffer)] = subKey;
					}
				}
			}

			/*idxBuffer = 0;
			for (auto it = vMainRenderPack->puSceneBuffers.begin(); it != vMainRenderPack->puSceneBuffers.end(); ++it)
			{
				ShaderKeyPtr subKey = it->second->GetShaderKey();
				if (subKey)
				{
					for (auto itInc = subKey->puIncludeFileNames.begin(); itInc != subKey->puIncludeFileNames.end(); ++itInc)
					{
						if (subKey->puParentCodeTree)
						{
							ShaderKeyPtr incKey = subKey->puParentCodeTree->GetIncludeKey(it->first);
							if (incKey)
							{
								auto arr = ct::splitStringToSet(incKey->GetPath(), "\\/");
								if (arr.find("predefined") == arr.end())
									keys["include." + ct::toStr(++idxInclude)] = incKey;
							}
						}
					}

					keys["buffer." + ct::toStr(++idxBuffer)] = subKey;
				}
			}*/

			CreateTemplate(vMainFileName, vPath, keys);
		}
	}
}

// on va creer un template cad faire les main et les childs
void TemplateSystem::CreateTemplate(const std::string& vFileName, const std::string& vFilePath, std::map<std::string, ShaderKeyPtr> vKeys)
{
	if (!vFileName.empty())
	{
		std::map<std::string, std::string> codes;
		std::map<std::string, std::string> names;
		std::map<std::string, bool> includes;

		// stock codes, and save old and new names
		for (auto it = vKeys.begin(); it != vKeys.end(); ++it)
		{
			auto code = FileHelper::Instance()->LoadFile(it->second->puKey, 0);
			if (!code.empty())
			{
				codes[it->first] = code;

				includes[it->first] = it->second->puIsInclude;

				if (it->second->puIsInclude)
				{
					if (!it->second->puIncludeFileInCode.empty())
					{
						names[it->second->puIncludeFileInCode] = it->first;
					}
				}
				else
				{
					auto unis = it->second->GetUniformsByWidget("buffer");
					if (unis)
					{
						for (auto itLst = unis->begin(); itLst != unis->end(); ++itLst)
						{
							if (!(*itLst)->bufferFileInCode.empty())
							{
								auto bufferName = (*itLst)->bufferShaderName;
								if (bufferName.find(".glsl") == std::string::npos)
								{
									bufferName += ".glsl";
								}

								// on va localiser la key
								for (auto itKey = vKeys.begin(); itKey != vKeys.end(); ++itKey)
								{
									if (itKey->second->puKey.find(bufferName) != std::string::npos)
									{
										names[(*itLst)->bufferFileInCode] = itKey->first;
										break;
									}
								}
							}
						}
					}
				}
			}
			else
			{
				LogVarError("[ERROR] Can't find the file file %s. Continue, but maybe, the template generation will not work properly.", it->second->puKey.c_str());
			}
		}

		// replace id in all codes
		std::string key, newkey;
		for (auto itFirstRing = codes.begin(); itFirstRing != codes.end(); ++itFirstRing)
		{
			for (auto itSecondRing = names.begin(); itSecondRing != names.end(); ++itSecondRing)
			{
				// replace newName->first by newName->second in code

				if (includes.find(itSecondRing->second) != includes.end() && includes[itSecondRing->second]) // found
				{
					key = itSecondRing->first;
					newkey = "[[" + itSecondRing->second + "]]";
				}
				else
				{
					key = itSecondRing->first;
					newkey = key;
					ct::replaceString(newkey, " ", "");
					auto arr = ct::splitStringToVector(newkey, "=");
					if (arr.size() == 2)
					{
						newkey = key;
						ct::replaceString(newkey, arr[1], "[[" + itSecondRing->second + "]]");
					}
				}

				ct::replaceString(itFirstRing->second, key, newkey);
			}
		}

		// save files
		auto newFileName = vFileName;
		ct::replaceString(newFileName, ".glsl", "");
		for (auto it = codes.begin(); it != codes.end(); ++it)
		{
			auto code = it->second;
			auto path = vFilePath + FileHelper::Instance()->puSlashType + newFileName + "." + it->first;
			if (!code.empty())
			{
				FileHelper::Instance()->SaveToFile(code, path, 0);

			}
			else
			{
				LogVarError("[ERROR] Can't create template file for key %s to path %s", it->first.c_str(), path.c_str());
			}
		}

		// on maj les templates
		ScanTemplateDir("templates");
	}
}