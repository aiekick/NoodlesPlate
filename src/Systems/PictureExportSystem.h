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
#include <Gui/GuiBackend.h>
#include <ImGuiPack.h>
#include <Headers/RenderPackHeaders.h>

class FrameBuffer;
class RenderPack;
class PictureExportSystem : public conf::ConfigAbstract
{
private:
	bool puTakePictureFromFBOForEachCodeUpdate = false;
	bool puTakePictureFromScreenForEachCodeUpdate = false;
	bool puAlwaysShowShotButton = false;
	bool puTakePictureFromScreenNeeded = false;

public:
	bool puExport_Texture_FlipY = true;
	int puExport_Texture_Sample_Count = 0;

public:
	static PictureExportSystem* Instance()
	{
		static PictureExportSystem _instance;
		return &_instance;
	}

protected:
	PictureExportSystem(); // Prevent construction
	PictureExportSystem(const PictureExportSystem&) {}; // Prevent construction by copying
	PictureExportSystem& operator =(const PictureExportSystem&) { return *this; }; // Prevent assignment
	~PictureExportSystem(); // Prevent unwanted destruction

public:
	void ExportRenderPackToPictureFile(RenderPackWeak vRP, const std::string& vFilePathName, int vBufferId);
	void ExportFBOToPictureFile(FrameBufferPtr vFBO, const std::string& vFilePathName, int vBufferId);

	void ExportPictureDialogOptions(std::string vFilter, IGFDUserDatas vUserDatas, bool *vCantContinue);
	std::string ModifyFilePathNameWithTimeStamp(const std::string& vFilePathName, std::string vExt);
	void TakeScreenShotForEachModif(bool vForceShot);
	bool Is_TakeScreenSHotForEachModif_Needed();
	void ScreenGrabbIfNeeded(const GuiBackend_Window& vWin);
	void TakeScreenShotFromCurrentRenderPackWithTimeStamp(RenderPackWeak vRp);
	void TakeScreenShotFromSceenWithTimeStamp(const std::string& vFilePathName);

public: // imgui
	void DrawButton_Shot();
	void DrawMenu();

public:
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;

};
