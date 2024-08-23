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

#include <ImGuiPack/ImGuiPack.h>
#include <ctools/ConfigAbstract.h>
#include <ImGuiPack/ImGuiPack.h>

#include <string>

class ProjectFile;
class MessagePane : public AbstractPane, public conf::ConfigAbstract
{
private:
	bool puIsThereSomeShaderErrors = false;
	bool puIsThereSomeShaderWarnings = false;
	bool puShowErrors = true;
	bool puShowWarnings = true;

public:
	bool Init() override;
	void Unit() override;
	bool DrawPanes(const uint32_t& /*vCurrentFrame*/, bool* vOpened, ImGuiContext* /*vContextPtr*/ = nullptr, void* vUserDatas = nullptr) override;
    
	// configuration
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;

	bool IsThereSomeMessages(bool vCheckOnlyErrors);

	void DrawToolBarButtons(float vWidth, ImFont* vFontSymbol);

public: // singleton
	static std::shared_ptr<MessagePane> Instance()
	{
		static auto _instance = std::make_shared<MessagePane>();
		return _instance;
	}

public:
	MessagePane(); // Prevent construction
	MessagePane(const MessagePane&) {}; // Prevent construction by copying
	MessagePane& operator =(const MessagePane&) { return *this; }; // Prevent assignment
    virtual ~MessagePane();  // Prevent unwanted destruction};
};

