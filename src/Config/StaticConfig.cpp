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

#include "StaticConfig.h"
#include <ctools/cTools.h>

StaticConfig::StaticConfig()
{
	puUseTransparancy = false;
}

std::string StaticConfig::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	str += vOffset + "<useTransparancy>" + ct::toStr(puUseTransparancy ? "true" : "false") + "</useTransparancy>\n";
	str += vOffset + "<globalUIScale>" + ct::toStr(puGlobalUIScale) + "</globalUIScale>\n";
	str += vOffset + "<enableDPISupport>" + ct::toStr(puEnableDPISupport ? "true" : "false") + "</enableDPISupport>\n";

	return str;
}

bool StaticConfig::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	// The value of this child identifies the name of this element
	std::string strName = "";
	std::string strValue = "";
	std::string strParentName = "";

	strName = vElem->Value();
	if (vElem->GetText())
		strValue = vElem->GetText();
	if (vParent != nullptr)
		strParentName = vParent->Value();

	if (strName == "useTransparancy")
		puUseTransparancy = ct::ivariant(strValue).GetB();
	else if (strName == "globalUIScale")
		puGlobalUIScale = ct::fvariant(strValue).GetF();
	else if (strName == "enableDPISupport")
		puEnableDPISupport = ct::ivariant(strValue).GetB();

	return true;
}