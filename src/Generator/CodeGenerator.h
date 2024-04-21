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

#include <ctools/ConfigAbstract.h>
#include <SoGLSL/src/Headers/RenderPackHeaders.h>

/*
on doit utiliser cette classe dans les cas :
- quand on veut charger des parametres au plus tot dans le soft
- quand on veut avoir des parametres globaux li� au soft et non a une classe
*/

class CodeGenerator
{
public:
	static CodeGenerator* Instance()
	{
		static CodeGenerator _instance;
		return &_instance;
	}

protected:
	CodeGenerator(); // Prevent construction
	CodeGenerator(const CodeGenerator&) {}; // Prevent construction by copying
	CodeGenerator& operator =(const CodeGenerator&) { return *this; }; // Prevent assignment
	~CodeGenerator(); // Prevent unwanted destruction

public:
	bool CreateFilePathName(const std::string& vFilePathName, const std::string& vType, const std::string& vFilePath = "", bool useStandardPaths = true);

private:
	// Noodlesplate Core
	ShaderInfos Get_Shader_3DAxis();
	ShaderInfos Get_Shader_3DGrid();
	ShaderInfos Get_Shader_Mesh();
	ShaderInfos Get_Shader_GizmoCulling();
	ShaderInfos Get_Helper_Culling();
	ShaderInfos Get_Helper_Space3D();

	// Main Types
    ShaderInfos Get_Shader_Quad();
    ShaderInfos Get_Shader_Font();
	ShaderInfos Get_Shader_Points_2D();
	ShaderInfos Get_Shader_Points_3D_Lines();
	ShaderInfos Get_Shader_Points_3D_Triangles();
	ShaderInfos Get_Shader_Mesh_Base();
	ShaderInfos Get_Shader_Mesh_Tesselation();
	ShaderInfos Get_Shader_Mesh_Geometry();
	ShaderInfos Get_Shader_Mesh_Capture();

	// web sites base formats
	ShaderInfos Get_Shader_ShaderToy_Image();
	ShaderInfos Get_Shader_ShaderToy_Buffer();
	ShaderInfos Get_Shader_GlslSandbox_Base();
	ShaderInfos Get_Shader_GlslSandbox_MultiPass();
	ShaderInfos Get_Shader_GlslSandbox_SurfacePosition();
	ShaderInfos Get_Shader_VertexShaderArt_Base();

	// Uniforms help
	ShaderInfos Get_Shader_Uniforms_Help();

	// features shader
	ShaderInfos Get_Shader_Demo_Gizmo();
	ShaderInfos Get_Shader_Demo_Space3D();
	ShaderInfos Get_Shader_Demo_Gizmo_Culling();
	ShaderInfos Get_Shader_Demo_Buffer_Attachments();
	ShaderInfos Get_Shader_Demo_Compute();
	ShaderInfos Get_Shader_Demo_Instanced();
	ShaderInfos Get_Shader_Demo_Scene();
	ShaderInfos Get_Shader_Demo_Multipass_One_File_2_buffers();
	ShaderInfos Get_Shader_Demo_Multipass_One_File_3_buffers();
	ShaderInfos Get_Shader_Demo_Multipass_One_File_4_buffers();
};
