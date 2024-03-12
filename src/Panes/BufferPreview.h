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


#include <string>
#include <ctools/ConfigAbstract.h>
#include <Headers/RenderPackHeaders.h>
#include <AbstractPane.h>
#include <ImGuiFileDialog.h>

class RenderPack;
class ProjectFile;
class BufferPreview : public AbstractPane, public conf::ConfigAbstract
{
public:
	bool Init() override;
	void Unit() override;
	bool DrawPanes(const uint32_t& vCurrentFrame, PaneFlags& vInOutPaneShown, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    bool CanWeDisplay() override;

	// configuration
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
	
private:
	void DrawBufferPipe(RenderPackWeak vRP, const char* vLabel, const float& aw);
	int DrawBufferChain(RenderPackWeak vRoot, RenderPackWeak vRP, const std::set<std::string>& vBufferNames, const float& aw);

public: // singleton
	static std::shared_ptr<BufferPreview> Instance()
	{
		static auto _instance = std::make_shared<BufferPreview>();
		return _instance;
	}

public:
	BufferPreview(); // Prevent construction
	BufferPreview(const BufferPreview&) {}; // Prevent construction by copying
	BufferPreview& operator =(const BufferPreview&) { return *this; }; // Prevent assignment
	~BufferPreview(); // Prevent unwanted destruction};
};

