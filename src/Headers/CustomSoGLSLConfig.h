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


// codes from sdfmFont.h
#define ICON_SDFM_TRASH_CAN_OUTLINE u8"\ufa79"
#define ICON_SDFM_PAUSE u8"\uf3e4"
#define ICON_SDFM_PLAY u8"\uf40a"
#define ICON_SDFM_PENCIL u8"\uf3eb"
#define ICON_SDFM_LOCK u8"\uf33e"

#define SOGLSL_ICON_LABEL_RESET ICON_SDFM_TRASH_CAN_OUTLINE
#define SOGLSL_ICON_LABEL_PAUSE ICON_SDFM_PAUSE
#define SOGLSL_ICON_LABEL_PLAY ICON_SDFM_PLAY
#define SOGLSL_ICON_LABEL_EDIT ICON_SDFM_PENCIL
#define SOGLSL_ICON_LABEL_LOCK ICON_SDFM_LOCK

// colors for RenderDoc
#define QUAD_SHADER_PASS_DEBUG_COLOR ct::fvec4(0.6f, 0.8f, 0.9f, 0.5f)
#define MESH_SHADER_PASS_DEBUG_COLOR ct::fvec4(0.7f, 0.9f, 0.6f, 0.5f)
#define VERTEX_SHADER_PASS_DEBUG_COLOR ct::fvec4(0.7f, 0.4f, 0.6f, 0.5f)
#define COMPUTE_SHADER_PASS_DEBUG_COLOR ct::fvec4(0.7f, 0.6f, 0.9f, 0.5f)
#define GENERIC_RENDERER_DEBUG_COLOR ct::fvec4(0.8f, 0.8f, 0.5f, 0.5f)
#define IMGUI_RENDERER_DEBUG_COLOR ct::fvec4(0.9f, 0.6f, 0.6f, 0.5f)
#define SHADERBLOCK_SHADER_PASS_DEBUG_COLOR ct::fvec4(0.3f, 0.6f, 0.6f, 0.5f)
