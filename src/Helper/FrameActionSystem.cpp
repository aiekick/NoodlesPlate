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

// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FrameActionSystem.h"
#include <Project/ProjectFile.h>

void FrameActionSystem::Insert(ActionStamp vAction)
{
	if (vAction)
		puActions.push_front(vAction);
}

void FrameActionSystem::Add(ActionStamp vAction)
{
	if (vAction)
		puActions.push_back(vAction);
}

void FrameActionSystem::Clear()
{
	puActions.clear();
}

void FrameActionSystem::RunActions()
{
	if (!puActions.empty())
	{
		const auto action = *puActions.begin();
		if (action()) // one action per frame, it true we can continue by deleting the current
		{
			if (!puActions.empty()) // because an action can clear actions
			{
				puActions.pop_front();
			}
		}
	}
}