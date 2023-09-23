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
#include <Headers/Globals.h>
#include <ctools/ConfigAbstract.h>
#include <Project/ProjectFile.h>
#include <Gui/MainToolBar.h>
#include <SoGLSL/src/Texture/Texture2D.h>

/*
il faut separer vue et model
 * la vue c'ets le frontend
 * le model c'est le backend

frontend => MainFrame
backend => MainBackend
*/

class FileDialogEntity : public conf::ConfigAbstract
{
public:
	std::string filePathName;
	std::string filePath;
	std::string src_ext;
	std::string tgt_ext;
	std::string type;

public:
	FileDialogEntity()
	{
		filePath = ".";
	}

	void SetType(const std::string& vType)
	{
		type = vType;
	}

	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override
	{
		UNUSED(vUserDatas);

		assert(!type.empty());

		std::string str;
		str += vOffset + "<" + type + ">\n";
		str += vOffset + "\t<filepathname>" + filePathName + "</filepathname>\n";
		str += vOffset + "\t<filepath>" + filePath + "</filepath>\n";
		str += vOffset + "\t<src_ext>" + src_ext + "</src_ext>\n";
		str += vOffset + "\t<tgt_ext>" + tgt_ext + "</tgt_ext>\n";
		str += vOffset + "</" + type + ">\n";
		return str;
	}

	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override
	{
		UNUSED(vUserDatas);

		assert(!type.empty());

		// The value of this child identifies the name of this element
		std::string strName = "";
		std::string strValue = "";
		std::string strParentName = "";

		strName = vElem->Value();
		if (vElem->GetText())
			strValue = vElem->GetText();
		if (vParent != nullptr)
			strParentName = vParent->Value();

		if (strParentName == type)
		{
			if (strName == "filepathname")
				filePathName = strValue;
			if (strName == "filepath")
				filePath = strValue;
			if (strName == "src_ext")
				src_ext = strValue;
			if (strName == "tgt_ext")
				tgt_ext = strValue;
		}

		return true;
	}
};

class ShaderKey;
class RenderPack;
class FrameBuffer;
class CameraSystem;
class UniformVariant;
class UniformsMultiLoc;
class FrameBuffersPipeLine;

struct GLFWmonitor;
class MainFrame : public conf::ConfigAbstract
{
public:
	static std::string puFileToLoad;
	static bool sAnyWindowsHovered;
	static bool sCentralWindowHovered;

public:
	bool puShowAboutDialog = false;	// montre la boit de dialogue a propos
	bool puShowDebug = false;		// montre les dffierent truc de debug (comme les uniforms)
#ifdef USE_HELP_IN_APP
	bool puShowScriptHelp = false; // montre le panneau d'aide
#endif
	bool puShowImGui = false; // montre le panneau du nodegraph
	bool puShowMetric = false;

private:
	ct::ivec4 puCentralPaneSize;

protected:
	FileDialogEntity puOpenFileDialog;
	FileDialogEntity puNewFileDialog;
	FileDialogEntity puSavePictureDialog;
	FileDialogEntity puForkFileDialog;
	MainToolBar puMainToolBar;
    Texture2DPtr m_ShadowTexturePtr = nullptr;

private:
	bool puShowImportBar = false;
	bool puShowErrorDialog = false;
	bool puConsoleGuiVisiblity = false;
	bool puShowUI = true;
	uint32_t puShowUIFrame = 0U; // frame id when puShowUI was set
	std::string puErrorDialogTitle;
	std::string puErrorDialogMsg;
	ProjectFile puProjectFile;

public:
	bool Init();
	void Unit();
	void NewFrame();
	void Display(ct::ivec2 vSize);
	void OpenImportBar(std::string vUrl = "");

private: // imgui
	void DrawSettingsMenu_Global(); // Settings menu Global
	bool DrawImportBar();
	bool DrawErrorDialog();
	bool DoAbout();
	void DrawMainMenuBar();
	void DrawFileDialogs(const ct::ivec2& vScreenSize);
	void CreateNewShader(std::string vType, const std::string& vFilePathName);
	void CanMouseAffectRendering();
	void ShowAboutDialog();
    bool m_build_themes();

public:  // configuration
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;

public:
	static MainFrame* Instance()
	{
		static MainFrame _instance;
		return &_instance;
	}

protected:
	MainFrame(); // Prevent construction
	MainFrame(const MainFrame&) {}; // Prevent construction by copying
	MainFrame& operator =(const MainFrame&) { return *this; }; // Prevent assignment
	~MainFrame(); // Prevent unwanted destruction
};
