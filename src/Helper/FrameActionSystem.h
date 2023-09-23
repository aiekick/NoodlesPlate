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

#include <list>
#include <functional>

/*
 list of sequential actions
 executed time to time only if executed action retrun true
 each succesfull action is erased
 not sucessfull action is applied again each frame
*/

class ProjectFile;
class FrameActionSystem
{
private:
	typedef std::function<bool()> ActionStamp;
	std::list<ActionStamp> puActions;

public:
	// insert an action at first, cause :
	// this action will be executed first at the next frame
	void Insert(ActionStamp vAction);
	// add an action at end
	// this action will be executed at least after all others
	void Add(ActionStamp vAction);
	// clear all actions
	void Clear();
	// apply first action each frame until true is returned
	// if return true, erase action
	// let the next frame call the next action
	// il false, action executed until true
	void RunActions();
};