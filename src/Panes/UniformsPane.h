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
#include <ctools/ConfigAbstract.h>
#include <Headers/RenderPackHeaders.h>

#include <string>

class UniformsMultiLoc;
class RenderPack;
class ProjectFile;
class UniformsPane : public AbstractPane, public conf::ConfigAbstract
{
public:
	bool Init() override;
	void Unit() override;
	bool DrawPanes(const uint32_t& /*vCurrentFrame*/, bool* vOpened, ImGuiContext* /*vContextPtr*/ = nullptr, void* vUserDatas = nullptr) override;
    
	// configuration
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
	
private:
	void DrawImGuiRenderPackCategory(RenderPackWeak vRp, bool vCheckMode, bool vShowEditBtn, ct::ivec2 vScreenSize);
	void DrawImGuiUniformWidget(UniformsMultiLoc* vUniLoc);

public: // singleton
	static std::shared_ptr<UniformsPane> Instance()
	{
		static auto _instance = std::make_shared<UniformsPane>();
		return _instance;
	}

public:
	UniformsPane(); // Prevent construction
	UniformsPane(const UniformsPane&) {}; // Prevent construction by copying
	UniformsPane& operator =(const UniformsPane&) { return *this; }; // Prevent assignment
    virtual ~UniformsPane();  // Prevent unwanted destruction};
};

