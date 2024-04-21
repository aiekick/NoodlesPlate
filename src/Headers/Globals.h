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

//DEFINES
//#define MESHIZER_VERBOSE_MODE
//#define USE_EMBEDDED_SCRIPTS
#define SAVE_SHADERS_TO_FILE_FOR_DEBUG_WHEN_ERRORS
#define SAVE_SHADERS_TO_FILE_FOR_DEBUG_WHEN_NO_ERRORS
//#define SHOW_LOGS_FOR_TEMPLATES
#define USE_HELP_IN_APP
#define USE_COOLBAR
//#define USE_NODEGRPAH
//#define USE_DESKTOP_TEXTURE
#define USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
//#define USE_SOUNDS

// RenderPacks
#define RP_MAIN 0
#define RP_GRID3D 1
#define RP_AXIS3D 2
#define RP_MESH 3

// VARS
#define MAX_CONFIG_COUNT_PER_SHADER_TYPE 1000
#define MAX_UNIFORM_COMMENT_BUFFER_SIZE 512
#define MAX_UNIFORM_BUFFER_SIZE 1024 
#define DEFAULT_RENDERING_QUALITY 1.0f

#define HEADER_PANE_HEIGHT 32.0f
#define CONSOLE_PANE_HEIGHT 32.0f
#define PALETTE_PANE_WIDTH 120.0f
#define MAX_SEARCH_SIZE 255
#define MAX_CHARSET_SIZE 1024
#define MAX_OPENCL_BUFFER_ERROR_SIZE 2048 
#define MAX_SHADER_BUFFER_ERROR_SIZE 2048 
#define MAX_FILE_DIALOG_NAME_BUFFER 1024
#define SHADER_UNIFORM_FIRST_COLUMN_WIDTH 150.0f
#define TRANSPARENCY_DAMIER_ZOOM 5.0f

// moyenne du calcul du fps sur COUNT_ITEM_FOR_FPS_MEAN elements
#define COUNT_ITEM_FOR_FPS_MEAN 5

#define TEMPLATE_2D_QUAD "Template 2D Quad"
#define TEMPLATE_2D_QUAD_TEXTURE2D "Template 2D Quad With Texture 2D"
#define TEMPLATE_3D_MESH "Template 3D Mesh"
#define TEMPLATE_3D_POINTS "Template 3D Points"
#define TEMPLATE_EMPTY "Template Empty"
#define TEMPLATE_SHADERTOY "Template ShaderToy"
#define TEMPLATE_GLSLSANDBOX "Template GlslSandbox"

// type special de template
#define TEMPLATE_USER "User Template"

#define SHADER_VERTEX_SHEET_NAME "Vertex"
#define SHADER_FRAGMENT_SHEET_NAME "Fragment"
#define SHADER_GEOMETRY_SHEET_NAME "Geometry"
#define SHADER_NOTES_SHEET_NAME "Notes"

// SdfFontDesigner
#define defaultCharSet "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789\n?.*+-/_'()&:,=!\" \\@"
#define TRANSPARENCY_DAMIER_ZOOM 5.0f
enum AlgoEnum {
    ALGO_SDF = 0,  // MSDFGEN SDF
    ALGO_PSDF,     // MSDFGEN PSEUDO SDF
    ALGO_MSDF      // MSDFGEN MSDF
};
enum FontTextureTypeEnum {
    FONT_TEXTURE_TYPE_BASE = 0,  // SDF FONT TEXTURE
    FONT_TEXTURE_TYPE_RENDERED   // SHADER FONT TEXTURE
};
enum FontLoadingModeEnum {
    FONT_LOADING_FROM_SYSTEM = 0,  // FONT from system32/fonts on windows
    FONT_LOADING_FROM_FILE         // FONT from file path name
};