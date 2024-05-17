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

// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "CodeGenerator.h"
#include <ctools/FileHelper.h>

CodeGenerator::CodeGenerator() {
}

CodeGenerator::~CodeGenerator() {
}

bool CodeGenerator::CreateFilePathName(const std::string& vFilePathName,
                                       const std::string& vType,
                                       const std::string&
                                       /*vFilePath*/,
                                       bool useStandardPaths) {
    if (!vFilePathName.empty()) {
        const auto ps = FileHelper::Instance()->ParsePathFileName(vFilePathName);
        if (ps.isOk) {
            FileHelper::Instance()->CreatePathIfNotExist(ps.path);
        }

        ShaderInfos infos;

        // Noodlesplate Core
        if (vType == "Shader_3dAxis")
            infos = Get_Shader_3DAxis();
        else if (vType == "Shader_3dGrid")
            infos = Get_Shader_3DGrid();
        else if (vType == "Shader_Mesh")
            infos = Get_Shader_Mesh();
        else if (vType == "Shader_Gizmoculling")
            infos = Get_Shader_GizmoCulling();
        else if (vType == "Helper_Culling")
            infos = Get_Helper_Culling();
        else if (vType == "Helper_Space3d")
            infos = Get_Helper_Space3D();

        // main types
        else if (vType == "Shader_Quad")
            infos = Get_Shader_Quad();
        else if (vType == "Shader_Font_Base")
            infos = Get_Shader_Font_Base();
        else if (vType == "Shader_Font_Glyph")
            infos = Get_Shader_Font_Glyph();
        else if (vType == "Shader_Points_2D")
            infos = Get_Shader_Points_2D();
        else if (vType == "Shader_Points_3D_Lines")
            infos = Get_Shader_Points_3D_Lines();
        else if (vType == "Shader_Points_3D_Triangles")
            infos = Get_Shader_Points_3D_Triangles();
        else if (vType == "Shader_Mesh_Base")
            infos = Get_Shader_Mesh_Base();
        else if (vType == "Shader_Mesh_Tesselation")
            infos = Get_Shader_Mesh_Tesselation();
        else if (vType == "Shader_Mesh_Geometry")
            infos = Get_Shader_Mesh_Geometry();
        else if (vType == "Shader_Mesh_Capture")
            infos = Get_Shader_Mesh_Capture();

        // web sites base formats
        else if (vType == "Shader_ShaderToy_Image")
            infos = Get_Shader_ShaderToy_Image();
        else if (vType == "Shader_ShaderToy_Buffer")
            infos = Get_Shader_ShaderToy_Buffer();
        else if (vType == "Shader_GlslSandbox_Base")
            infos = Get_Shader_GlslSandbox_Base();
        else if (vType == "Shader_GlslSandbox_MultiPass")
            infos = Get_Shader_GlslSandbox_MultiPass();
        else if (vType == "Shader_GlslSandbox_SurfacePosition")
            infos = Get_Shader_GlslSandbox_SurfacePosition();
        else if (vType == "Shader_VertexShaderArt_Base")
            infos = Get_Shader_VertexShaderArt_Base();

        // all uniforms types
        else if (vType == "Shader_Uniforms_Help")
            infos = Get_Shader_Uniforms_Help();

        // features shader
        else if (vType == "Shader_Demo_Gizmo")
            infos = Get_Shader_Demo_Gizmo();
        else if (vType == "Shader_Demo_Space3D")
            infos = Get_Shader_Demo_Space3D();
        else if (vType == "Shader_Demo_Gizmo_Culling")
            infos = Get_Shader_Demo_Gizmo_Culling();
        else if (vType == "Shader_Demo_Buffer_Attachments")
            infos = Get_Shader_Demo_Buffer_Attachments();
        else if (vType == "Shader_Demo_Compute")
            infos = Get_Shader_Demo_Compute();
        else if (vType == "Shader_Demo_Instanced_Mesh")
            infos = Get_Shader_Demo_Instanced();
        else if (vType == "Shader_Demo_Scene")
            infos = Get_Shader_Demo_Scene();
        else if (vType == "Shader_Demo_Multipass_One_File_2_buffers")
            infos = Get_Shader_Demo_Multipass_One_File_2_buffers();
        else if (vType == "Shader_Demo_Multipass_One_File_3_buffers")
            infos = Get_Shader_Demo_Multipass_One_File_3_buffers();
        else if (vType == "Shader_Demo_Multipass_One_File_4_buffers")
            infos = Get_Shader_Demo_Multipass_One_File_4_buffers();

        const auto file = infos.GetCode();

        if (!file.empty()) {
            const auto fpn = FileHelper::Instance()->CorrectSlashTypeForFilePathName(vFilePathName);
            FileHelper::Instance()->SaveToFile(file, fpn, useStandardPaths ? (int)FILE_LOCATION_Enum::FILE_LOCATION_SCRIPT : (int)FILE_LOCATION_Enum::FILE_LOCATION_NONE);

            return true;
        }
    }

    return false;
}

// NoodlesPlate Core

ShaderInfos CodeGenerator::Get_Shader_3DAxis() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform float(maxpoints) uCountVertex;
uniform mat4(camera:mvp) cam;
uniform float(0.0:20.0:10.0) uAxisSize;

@NOTE

[[NAME]]: 3D Axis
[[DATE]]: 04 April 2018

@VERTEX POINTS(6) DISPLAY(LINES)

layout(location = 0) in float vertexId;
out vec4 v_color;

void main()
{
	float index = vertexId;

	vec3 p = vec3(0);

	// z = blue
	if (index == 0.) p = vec3(0.0, 0.0, 0.0), v_color = vec4(0.0, 0.0, 1.0, 1.0);
	if (index == 1.) p = vec3(0.0, 0.0, uAxisSize), v_color = vec4(0.0, 0.0, 1.0, 1.0);

	// y = green
	if (index == 2.) p = vec3(0.0, 0.0, 0.0), v_color = vec4(0.0, 1.0, 0.0, 1.0);
	if (index == 3.) p = vec3(0.0, uAxisSize, 0.0), v_color = vec4(0.0, 1.0, 0.0, 1.0);

	// x = red
	if (index == 4.) p = vec3(0.0, 0.0, 0.0), v_color = vec4(1.0, 0.0, 0.0, 1.0);
	if (index == 5.) p = vec3(uAxisSize, 0.0, 0.0), v_color = vec4(1.0, 0.0, 0.0, 1.0);

	gl_Position = cam * vec4(p, 1);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;
in vec4 v_color;

void main(void)
{
	fragColor = v_color;
	fragColor.a = 1.0;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_3DGrid() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform float(maxpoints) uCountVertex;
uniform mat4(camera:mvp) cam;
uniform float(0.0:2.0:1.0) uGridSize;
uniform float(0.0:20.0:10.0) uGridCount;
uniform vec3(color:0.8) uGridColor;

@NOTE

[[NAME]]: 3D Grid
[[DATE]]: 04 April 2018

@VERTEX POINTS(200) DISPLAY(LINES)

layout(location = 0) in float vertexId;
out vec4 v_color;

void main()
{
	float index = mod(vertexId, 4.);

	float indexAxis = floor(vertexId / 4.);

	if (indexAxis > uGridCount * 2.0 || 
		vertexId > uCountVertex)
	{
		gl_Position = vec4(0.0);
		v_color = vec4(0.0);
		return;
	}
	
	float gridCount = floor(uGridCount * 2.) / 2.;

	vec2 p = vec2(0);

	vec2 base = vec2(indexAxis * 0.5 - gridCount * 0.5);
	vec2 limit = vec2(gridCount * 0.5);

	if (index == 0.) p = vec2(-limit.x, base.y);
	if (index == 1.) p = vec2(limit.x, base.y);
	if (index == 2.) p = vec2(base.x, -limit.y);
	if (index == 3.) p = vec2(base.x, limit.y);

	p *= uGridSize;

	gl_Position = cam * vec4(p.x, 0.0, p.y, 1);
	v_color = vec4(uGridColor, 1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;
in vec4 v_color;

void main(void)
{
	fragColor = v_color;
	fragColor.a = 1.0;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Mesh() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform(hidden) mat4(camera:mvp) _cam;
uniform(hidden) mat4(camera:v) _view;
uniform(hidden) mat4(camera:p) _proj;
uniform(hidden) mat4(camera:m) _model;
uniform(hidden) mat4(camera:nm) _normal;
uniform(hidden) vec2(buffer:0:SliceMesh) screenSize;

uniform(matcap:0) float(checkbox:true) useMatCap;
uniform(matcap:1:useMatCap==true) sampler2D(picture:choice) uMatCap;
uniform(matcap:2:useMatCap==true) float(0.0:1.0:1.0) RatioMatCatCol;

uniform(Geometry) float(0.0:1.0:0.0) geomNormalLen;
uniform(Geometry) float(0.0:1.0:0.0) geomFaceInset;
uniform(Geometry) float(0.0:1.0:0.0) geomFaceOffset;
uniform(Geometry) float(1.0:5.0:0.0) geomPointSize;

uniform(normals:0) float(checkbox) calcNormals;
uniform(normals:1:calcNormals==true) float(0.0:1.0:0.0) RatioNorCol;
uniform(normals:2) float(checkbox) useNormalAsColor;

uniform(vertex:0) float(checkbox) useVertexColor;
uniform(vertex:1:useVertexColor==true) float(0.0:1.0:0.0) RatioVertCol;

@COMMON

vec2 getMatCap(vec3 pos, vec3 nor)
{
	mat4 modelViewMatrix = _view * _model;
	mat4 normalMatrix = _normal;//transpose(inverse(modelViewMatrix));
	vec4 pp = modelViewMatrix * vec4( pos, 1. );
	vec4 nn = normalMatrix * vec4(nor, 1.);
		
	vec3 rd = normalize( pp.xyz );
	vec3 n = normalize( nn.xyz );

	vec3 r = reflect( rd, n );
	r.y *= -1.;
	r.z += 1.;
	float m = 2. * length(r) + 0.0001;
	return r.xy / m - 0.5;
}

@VERTEX MESH()

layout(location = 0) in vec3 a_position; // Current vertex position
layout(location = 1) in vec3 a_normal; // Current normal at this vertex
layout(location = 2) in vec2 a_uv; // Current uv at this vertex
layout(location = 3) in vec4 a_color; // Current color at this vertex
out vec3 v_position; // for have position in others shaders
out vec3 v_normal; // for have normal in others shaders
out vec2 v_uv; // for have vertex uv in others shaders
out vec4 v_color; // for have vertex color in others shaders

out vec3 faceNormal;
out vec3 v_pos;
out vec2 vN;

void main()
{
	gl_PointSize = 3.;
	v_position = a_position;
	v_normal = a_normal;
	v_uv = a_uv;
	v_color = a_color;
	gl_Position = _cam * vec4(a_position,1);
	v_pos = gl_Position.xyz;
	faceNormal = vec3(0);
	
	if (useMatCap > 0.5)
	{
		// matcap
		vN = getMatCap(v_position, v_normal);
	}
}

@GEOMETRY

layout(location = 0) in vec3 v_position[]; // for have position in others shaders
layout(location = 1) in vec3 v_normal[]; // for have normal in others shaders
layout(location = 2) in vec2 v_uv[]; // for have vertex uv in others shaders
layout(location = 3) in vec4 v_colors[]; // for have vertex color in others shaders

out vec4 v_color; // for have vertex color in others shaders
out vec2 vN;

@CONFIG_START Face Points

layout(triangles) in; // input primitive(s) (must match the current render mode)
layout(points, max_vertices=1) out; // output primitive(s)
out vec3 faceNormal;

void main()
{
	vN = vec2(0);
	gl_PointSize = geomPointSize;
	
	vec3 faceCenter = (v_position[0] + v_position[1] + v_position[2]) / 3.;
	faceNormal = (v_normal[0] + v_normal[1] + v_normal[2]) / 3.;
	
	if (calcNormals > 0.5)
	{
		faceNormal = normalize(
					cross(	v_position[0] - v_position[2],
							v_position[0] - v_position[1]));
	}
	
	if (useMatCap > 0.5)
	{
		vN = getMatCap(faceCenter, faceNormal);
	}
	
	gl_Position = _cam * vec4(faceCenter, 1.0);
	v_color = vec4(faceNormal * 0.5 + 0.5, 1.0);
	EmitVertex();
		
	EndPrimitive();
}

@CONFIG_END

@CONFIG_START Face Normals

layout(triangles) in; // input primitive(s) (must match the current render mode)
layout(line_strip, max_vertices=6) out; // output primitive(s)
out vec3 faceNormal;

void main()
{
	vN = vec2(0);
	vec3 faceCenter = (v_position[0] + v_position[1] + v_position[2]) / 3.;
	faceNormal = (v_normal[0] + v_normal[1] + v_normal[2]) / 3.;
	
	if (calcNormals > 0.5)
	{
		faceNormal = -normalize(
					cross(	v_position[0] - v_position[2],
							v_position[0] - v_position[1]));
	}
	
	if (useMatCap > 0.5)
	{
		vN = getMatCap(faceCenter, faceNormal);
	}
	
	v_color = vec4(0,0,0,1);
	gl_Position = _cam * vec4(v_position[0], 1.0);
	EmitVertex();
	
	gl_Position = _cam * vec4(v_position[1], 1.0);
	EmitVertex();
	
	gl_Position = _cam * vec4(v_position[2], 1.0);
	EmitVertex();
	
	gl_Position = _cam * vec4(v_position[0], 1.0);
	EmitVertex();
	
	v_color = vec4(faceNormal * 0.5 + 0.5, 1.0);
	
	gl_Position = _cam * vec4(faceCenter, 1.0);
	EmitVertex();

	gl_Position = _cam * vec4(faceCenter + faceNormal * geomNormalLen, 1.0);
	EmitVertex();
		
	EndPrimitive();
}

@CONFIG_END

@CONFIG_START Points Normals

layout(triangles) in; // input primitive(s) (must match the current render mode)
layout(line_strip, max_vertices=2) out; // output primitive(s)
out vec3 faceNormal;

void main()
{
	vN = vec2(0);
	
	if (useMatCap > 0.5)
	{
		vN = getMatCap(v_position[0], v_normal[0]);
	}
	
	//faceNormal = v_normal[0];
	
	v_color = vec4(v_normal[0] * 0.5 + 0.5, 1.0);
	
	gl_Position = _cam * vec4(v_position[0], 1.0);
	EmitVertex();
	
	gl_Position = _cam * vec4(v_position[0] + v_normal[0] * geomNormalLen, 1.0);
	EmitVertex();
		
	EndPrimitive();
}

@CONFIG_END

@CONFIG_START Mesh

layout(triangles) in; // input primitive(s) (must match the current render mode)
layout(line_strip, max_vertices=6) out; // output primitive(s)
out vec3 faceNormal;

void main()
{
	vN = vec2(0);
	faceNormal = (v_normal[0] + v_normal[1] + v_normal[2]) / 3.;
	
	if (calcNormals > 0.5)
	{
		faceNormal = -normalize(
					cross(	v_position[0] - v_position[2],
							v_position[0] - v_position[1]));
	}
	
	if (useMatCap > 0.5)
	{
		vN = getMatCap(v_position[0], faceNormal);
	}
							
	v_color = vec4(faceNormal * 0.5 + 0.5,1.0) * RatioNorCol;
	
	gl_Position = _cam * vec4(v_position[0], 1.0);
	v_color += v_colors[0] * RatioVertCol;
	EmitVertex();

	gl_Position = _cam * vec4(v_position[1], 1.0);
	v_color += v_colors[1] * RatioVertCol;
	EmitVertex();
		
	gl_Position = _cam * vec4(v_position[1], 1.0);
	v_color += v_colors[1] * RatioVertCol;
	EmitVertex();

	gl_Position = _cam * vec4(v_position[2], 1.0);
	v_color += v_colors[2] * RatioVertCol;
	EmitVertex();
		
	gl_Position = _cam * vec4(v_position[2], 1.0);
	v_color += v_colors[2] * RatioVertCol;
	EmitVertex();

	gl_Position = _cam * vec4(v_position[0], 1.0);
	v_color += v_colors[0] * RatioVertCol;
	EmitVertex();
		
	EndPrimitive();
}

@CONFIG_END

@CONFIG_START Face Inset

layout(triangles) in; // input primitive(s) (must match the current render mode)
layout(triangle_strip, max_vertices=3) out; // output primitive(s)
out vec3 faceNormal;

void main()
{
	vN = vec2(0);
	vec3 faceCenter = (v_position[0] + v_position[1] + v_position[2]) / 3.;
	faceNormal = (v_normal[0] + v_normal[1] + v_normal[2]) / 3.;
	
	if (calcNormals > 0.5)
	{
		faceNormal = -normalize(
					cross(	v_position[0] - v_position[2],
							v_position[0] - v_position[1]));
	}
	
	if (useMatCap > 0.5)
	{
		vN = getMatCap(faceCenter, faceNormal);
	}
	
	v_color = vec4(faceNormal * 0.5 + 0.5,1.0);
	
	vec3 p = v_position[0];
	vec3 vec = p - faceCenter;
	float len = length(vec);
	vec3 dir = vec / len;
		
	gl_Position = _cam * vec4(p + dir * len * -geomFaceInset, 1.0);
	v_color = v_colors[0];
	EmitVertex();
		
	p = v_position[1];
	vec = p - faceCenter;
	len = length(vec);
	dir = vec / len;
		
	gl_Position = _cam * vec4(p + dir * len * -geomFaceInset, 1.0);
	v_color = v_colors[1];
	EmitVertex();
		
	p = v_position[2];
	vec = p - faceCenter;
	len = length(vec);
	dir = vec / len;
		
	gl_Position = _cam * vec4(p + dir * len * -geomFaceInset, 1.0);
	v_color = v_colors[2];
	EmitVertex();
		
	EndPrimitive();
}

@CONFIG_END

@CONFIG_START Face Offset

layout(triangles) in; // input primitive(s) (must match the current render mode)
layout(triangle_strip, max_vertices=3) out; // output primitive(s)
out vec3 faceNormal;

void main()
{
	vN = vec2(0);
	vec3 faceCenter = (v_position[0] + v_position[1] + v_position[2]) / 3.;
	faceNormal = (v_normal[0] + v_normal[1] + v_normal[2]) / 3.;
	
	if (calcNormals > 0.5)
	{
		faceNormal = -normalize(
					cross(	v_position[0] - v_position[2],
							v_position[0] - v_position[1]));
	}
	
	if (useMatCap > 0.5)
	{
		vN = getMatCap(faceCenter, faceNormal);
	}
	
	v_color = vec4(faceNormal * 0.5 + 0.5,1.0);
	
	gl_Position = _cam * vec4(v_position[0] + faceNormal * geomFaceOffset, 1.0);
	EmitVertex();
		
	gl_Position = _cam * vec4(v_position[1] + faceNormal * geomFaceOffset, 1.0);
	EmitVertex();
		
	gl_Position = _cam * vec4(v_position[2] + faceNormal * geomFaceOffset, 1.0);
	EmitVertex();
		
	EndPrimitive();
}

@CONFIG_END

@FRAGMENT

in vec3 v_position; // for have position in others shaders
in vec3 v_normal; // for have normal in others shaders
in vec2 v_uv; // for have vertex color in others shaders
in vec4 v_color; // for have vertex color in others shaders

layout(location = 0) out vec4 fragColor;

in vec3 faceNormal;
in vec3 v_pos;
in vec2 vN;

#include "space3d.glsl"

void main()
{
	vec4 col = vec4(0,0,0,1);

	vec2 tN = vN;
	
	vec3 n = v_normal.xyz;
	if (dot(faceNormal,faceNormal) > 0.)
	{
		n = faceNormal;
	}
	else if (calcNormals > 0.5)
	{
		n = normalize(cross(dFdx(v_pos), dFdy(v_pos)));
		if (useMatCap > 0.5)
		{
			tN = getMatCap(v_position, n);
		}
	}
	
	vec2 uv = (gl_FragCoord.xy * 2.0 - screenSize) / min(screenSize.x, screenSize.y);
	
	vec3 ro = getRayOrigin();
	vec3 rd = getRayDirection();
	
	float alpha = v_color.a * RatioVertCol;
	
	if (useNormalAsColor>0.5)
		col.rgb = (n * 0.5 + 0.5) * RatioNorCol;
	
	if (useVertexColor>0.5)
		col += v_color * RatioVertCol;
	
	if (useMatCap > 0.5)
	{
		col.rgb += texture( uMatCap, tN ).rgb * RatioMatCatCol;
	}
	
	fragColor = col;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_GizmoCulling() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform float(maxpoints) uCountVertex;
uniform mat4(camera:mvp) _cam;

uniform int(cullingtype) cullingType; // hidden // 0=>cube, 1=>sphere
uniform float(0:20:2) cullingSize;
uniform float(0:20:5:1) subdivisions;
uniform float(0:100:50:1) circleSegments;
uniform mat4(culling) _CullingMatrix;

@VERTEX POINTS(15000) DISPLAY(LINES)

layout(location = 0) in float vertexId;
out vec4 v_color;

vec3 getRect(float id)
{
	float index = mod(id, 8.);
	float idx = floor(id/8.);
	
	if (index < 0.5) return vec3(0,0,idx);
	if (index < 1.5) return vec3(1,0,idx);
	if (index < 2.5) return vec3(1,0,idx);
	if (index < 3.5) return vec3(1,1,idx);
	if (index < 4.5) return vec3(1,1,idx);
	if (index < 5.5) return vec3(0,1,idx);
	if (index < 6.5) return vec3(0,1,idx);
	if (index < 7.5) return vec3(0,0,idx);
	
	return vec3(0);
}

bool getWireCube(float id, out vec3 p)
{
	vec3 pid = getRect(id);
	float idx = floor(pid.z/3.);
	
	if (idx > subdivisions + 1.)
	{
		gl_Position = vec4(0.0);
		v_color = vec4(0.0);
		return false;
	}
	
	idx /= subdivisions + 1.;
	
	float index = mod(pid.z, 3.);
	if (index < 0.5) p = vec3(idx, pid.y, pid.x);
	else if (index < 1.5) p = vec3(pid.x, idx, pid.y);
	else if (index < 2.5) p = vec3(pid.x, pid.y, idx);
	
	p -= 0.5;
	
	return true;
}


vec3 getCircle(float id)
{
	float pi2 = radians(360.);
	float n = circleSegments;
	
	float index = mod(id, 2.);
	float idxSegment = floor(id / 2.);
	
	float asp = pi2 / n; // angle step line
	float ap0 = asp * idxSegment; // angle line 0
	float ap1 = asp * (idxSegment + 1.); // angle line 0
	
  	float a = 0.0;
	
	if (index == 0.) a = ap0;
	if (index == 1.) a = ap1;
	
	float idxCircle = floor(idxSegment / n);
	
	return vec3(cos(a), sin(a), idxCircle);
}

bool getWireSphere(float id, out vec3 p)
{
	float coundSubdivisions = subdivisions + 1.;
	
	vec3 pid = getCircle(id);
	pid.z = pid.z;
	
	float idx = floor(pid.z/3.);
	
	if (idx > coundSubdivisions)
	{
		gl_Position = vec4(0.0);
		v_color = vec4(0.0);
		return false;
	}
	
	idx /= coundSubdivisions;
	idx -= 0.5;
	
	float r = 1.0;
	float a = cos(idx*1.5/r);
	float h = sin(a) * r;
	pid.xy *= h;
	
	float index = mod(pid.z, 3.);
	if (index < 0.5) p = vec3(idx, pid.y, pid.x);
	else if (index < 1.5) p = vec3(pid.x, idx, pid.y);
	else if (index < 2.5) p = vec3(pid.x, pid.y, idx);
	
	p *= 0.68;
	return true;
}

void main()
{
	vec3 p = vec3(0);
	bool res  = false;
	
	if (cullingType == 0) res = getWireCube(vertexId, p);
	else if (cullingType == 1) res = getWireSphere(vertexId, p);
	
	if (res == false) return;
	
	gl_Position = _cam * _CullingMatrix * vec4(p.x, p.y, p.z, 1);
	v_color = vec4(1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;
in vec4 v_color;

void main(void)
{
	fragColor = v_color;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Helper_Culling() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform float(useculling) _useGizmoCull; // hidden
uniform(_useGizmoCull==true) int(cullingtype) _cullingType; // hidden // 0=>cube, 1=>sphere
uniform(_useGizmoCull==true) mat4(culling) _GizmoCull; // _GizmoCullingVolume

@FRAGMENT

// Volume Culling

// based on http://iquilezles.org/www/articles/intersectors/intersectors.htm
void GetSpherePoint( in vec3 ro, in vec3 rd, in mat4 txx, out vec3 facePoint, out vec2 d, out vec3 nor)
{
	mat4 txi = inverse(txx);
	
	float ra = 0.57;
	
	vec3 ocn = (txi*vec4(ro,1.0)).xyz/ra;
    vec3 rdn = (txi*vec4(rd,0.0)).xyz/ra;
    
    float a = dot( rdn, rdn );
    float b = dot( ocn, rdn );
    float c = dot( ocn, ocn );
    float h = b*b - a*(c-1.0);
	
    if( h < 0.0 )
	{
		facePoint = ro; 
		d = vec2(1.0,-1.0);
	}
	
    h = sqrt(h);
	
    float d1 = (-b-h)/a;
    float d2 = (-b+h)/a;
	
	facePoint = ro + rd * d1;
	d = vec2(d1, d2);
	nor = normalize(facePoint -  txx[3].xyz);
}

void getBoxPoint(in vec3 ro, in vec3 rd, in mat4 txx, out vec3 facePoint, out vec2 d, out vec3 nor) 
{
	mat4 txi = inverse(txx);
	
	// convert from ray to box space
	vec3 rdd = (txi*vec4(rd,0.0)).xyz;
	vec3 roo = (txi*vec4(ro,1.0)).xyz;

	// ray-box intersection in box space
    vec3 m = 1.0/rdd;
    vec3 n = m*roo;
    vec3 k = abs(m) * 0.5;
	
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;

	float d1 = max( max( t1.x, t1.y ), t1.z ); // near
	float d2 = min( min( t2.x, t2.y ), t2.z ); // far
	
	if( d1 >= d2 || d2 <= 0.0)
	{
		facePoint = ro;
		d = vec2(1.0,-1.0);
	}

	facePoint = (txx * vec4(roo + rdd * d1, 1.0)).xyz;
	d = vec2(d1, d2);
	nor = -sign(rdd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
}

bool getCullingPoint(in vec3 ro, in vec3 rd, out vec3 facePoint, out vec2 d, inout float maxDistance, out vec3 nor)
{
	facePoint = ro;
	nor = vec3(0);
	
	bool res = false;
	
	if (_useGizmoCull > 0.5)
	{
		d = vec2(1.0,-1.0);
		if (_cullingType == 0) // box
		{
			getBoxPoint(ro, rd, _GizmoCull, facePoint, d, nor);
		}
		else if (_cullingType == 1) // sphere
		{
			GetSpherePoint(ro, rd, _GizmoCull, facePoint, d, nor);
		}
		
		if (d.y > d.x)
		{
			maxDistance = min(maxDistance, d.y - d.x);
			res = true;
		}
	}
	else
	{
		d = vec2(00);
		res = true;
	}
	
	return res;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Helper_Space3D() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform vec2(buffer:target=0) size;
uniform(noexport) mat4(camera:p) _proj;
uniform(noexport) mat4(camera:v) _view;
uniform(noexport) mat4(camera:m) _model; 

@FRAGMENT

vec3 getRayOrigin(mat4 vView, mat4 vModel);
vec3 getRayOrigin();
vec3 getRayDirection(vec2 vUV, mat4 vProj, mat4 vView, mat4 vModel);
vec3 getRayDirection(vec2 vUV);
vec3 getRayDirection();
void CalcDepth(mat4 vProj, mat4 vModel, vec3 vRayDir, float vDist);
void CalcDepth(vec3 vRayDir, float vDist);

// Get Ray Origin
// vView and vModel are view and model matrixs
vec3 getRayOrigin(mat4 vView, mat4 vModel)
{
	vec3 ro = vView[3].xyz + vModel[3].xyz;
	ro *= mat3(vView * vModel);
	return -ro;
}

vec3 getRayOrigin()
{
	return getRayOrigin(_view, _model);
}

// Get Ray Direction
// vUV is the normalized central uv without ratio correction (coord/size*2.-1.)
// vProj, vView and VModel are Projection, View and Model Matrixs
vec3 getRayDirection(vec2 vUV, mat4 vProj, mat4 vView, mat4 vModel)
{
	vec4 ray_clip = vec4(vUV.x, vUV.y, -1., 1.);
	vec4 ray_eye = inverse(vProj) * ray_clip;
	vec3 rd = normalize(vec3(ray_eye.x, ray_eye.y, -1.));
	rd *= mat3(vView * vModel);
	return rd;
}

vec3 getRayDirection(vec2 vUV)
{
	return getRayDirection(vUV, _proj, _view, _model);
}

vec3 getRayDirection()
{
	return getRayDirection(gl_FragCoord.xy / size * 2. - 1., _proj, _view, _model);
}

// Get the Zbuffer value of the current pixel
// vProj and VModel are Projection and Model Matrixs
// vRayDir is the Ray Direction / vDist is the distance from Ray Origin to the Object
void CalcDepth(mat4 vProj, mat4 vModel, vec3 vRayDir, float vDist)
{
	vec3 eyeFwd = vec3(0., 0., -1.) * mat3(vModel);
	float depth = vProj[3].z * dot(vRayDir, eyeFwd) / vDist - vProj[2].z;
	gl_FragDepth = (gl_DepthRange.diff * depth + gl_DepthRange.near + gl_DepthRange.far) * 0.5;
}

void CalcDepth(vec3 vRayDir, float vDist)
{
	CalcDepth(_proj, _model, vRayDir, vDist);
}
)";
    return infos;
}

// Main types

ShaderInfos CodeGenerator::Get_Shader_Quad() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER 

//FORMAT(float or byte)
//COUNT(1 to 8)
//WRAP(clamp or repeat or mirror)
//FILTER(linear or nearest)
//MIPMAP(false or true)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform float(time) uTime;
uniform int(frame) uFrame;
uniform float(deltatime) uDeltaTime;
uniform vec2(buffer:target=0) uSize;

@VERTEX

layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;

void main(void)
{
	vec2 uv = gl_FragCoord.xy / uSize;
	vec4 color = vec4(0);
	color.rg += cos(uv + uTime) * 0.5 + 0.5;
	color.gb += sin(uv + uTime) * 0.5 + 0.5;
	fragColor = color;
	fragColor.a = 1.0;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Font_Base() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER 

//FORMAT(float or byte)
//COUNT(1 to 8)
//WRAP(clamp or repeat or mirror)
//FILTER(linear or nearest)
//MIPMAP(false or true)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform(texture) vec2(sdf)				uAtlasSize; // sdf texture size

uniform(glyph) int(glyphcount) 			uCountGlyphs; // count glyphs
uniform(glyph) vec4(glyphrects) 		uGlyphRects[glyphcount]; // glyph rects : left, bottom, right, top
uniform(glyph) vec2(glyphcenter) 		uGlyphCenterOffsets[glyphcount]; // glyph center offset : x,y on range 0,0 to 1,1, default is center 0.5,0.5

@FRAGMENT

layout(location = 0) out vec4 fragColor;

[[FONT_CODE]]

void mainFontMap(vec2 fragCoord) {
	vec2 uv = fragCoord / uAtlasSize;
	for (int i = 0; i < uCountGlyphs; i++) {
		vec4 rc = uGlyphRects[i];			
		if (uv.x >= rc.x && uv.y >= rc.y && uv.x <= rc.z && uv.y <= rc.w) {
			vec2 glyphSize = rc.zw - rc.xy;
			vec2 glyphCoord = mod(uv - rc.xy, glyphSize) - glyphSize * 0.5;
			glyphCoord -= glyphSize * 0.5 * (uGlyphCenterOffsets[i] * 2.0 - 1.0);			
			fragColor = mainGlyph(i, glyphCoord, glyphSize, fragCoord, uAtlasSize);
			// glyph found so we keep gpu cycles if we break here
			break;
		}
	}
}

void main(void) {
	mainFontMap(gl_FragCoord.xy);
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Font_Glyph() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform(sdf) float(0.0:0.1:0.05) 		uSmoothing; // smooth edge
uniform(sdf) float(-0.4:0.4:0.0) 		uOutlineWidth; // border line thickness

uniform(common) int(frame) 				uFrame;	// frames
uniform(common) float(time) 			uTime; // time

uniform(texture) sampler2D(sdf)			uAtlas; // sdf texture
uniform(texture) vec2(sdf)				uAtlasSize; // sdf texture size

uniform(glyph) vec2(glyphpadding) 		uGlyphPadding; // global glyphs padding
uniform(glyph) int(glyphcount) 			uCountGlyphs; // count glyphs
uniform(glyph) float(glyphinversions) 	uGlyphInversions[glyphcount]; // glyph inversion ( true is > 0.5, false < 0.5)
uniform(glyph) vec4(glyphrects) 		uGlyphRects[glyphcount]; // glyph rects : left, bottom, right, top
uniform(glyph) vec2(glyphcenter) 		uGlyphCenterOffsets[glyphcount]; // glyph center offset : x,y on range 0,0 to 1,1, default is center 0.5,0.5

uniform(color) vec3(color:0)			colorStart; // start filling color
uniform(color) vec3(color:1)			colorEnd; // end filling color

uniform(sdf) float(-1:1:0) dilat;

@FONT

// https://github.com/Chlumsky/msdfgen
float median(vec3 rgb) 
{
    return max(min(rgb.r, rgb.g), min(max(rgb.r, rgb.g), rgb.b));
}

void mainGlyph(in int glyphIndex, in vec2 glyphCoord, in vec2 glyphSize, in vec2 texCoord, in vec2 texSize)
{
	vec2 uv = texCoord / texSize;
	vec4 font = texture(uAtlas, uv);
				
	if (uGlyphInversions[glyphIndex] > 0.5) {
		font = 1.0 - font; // inversion
	}
	
	float glyphSdf = median(font.rgb);
					
	// put char between [[ and ]] for adapt your shader for a particular char
	// example with the char c, we want to inverse here the sdf only for the char c
	// if (glyphIndex == [[c]])	{ glyphSdf = 1. - glyphSdf; } 
			 
	glyphSdf -= dilat;

	float smoothing = uSmoothing;
	float outlineWidth = uOutlineWidth;
	float outerEdgeCenter = 0.5 - outlineWidth;

	float alpha = smoothstep(outerEdgeCenter - smoothing, outerEdgeCenter + smoothing, glyphSdf);
	float border = smoothstep(0.5 - smoothing, 0.5 + smoothing, glyphSdf);

	vec4 col = vec4(
		mix(colorStart, colorEnd, border),
		alpha);
		
	fragColor = col;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Points_2D() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform float(time) uTime;
uniform vec2(buffer) uSize;

@VERTEX POINTS(10000) DISPLAY(TRIANGLES)

layout(location = 0) in float vertexId;
out vec4 v_color;

void main()
{
	float astep = 3.14159 * 2.0 / 70.;
	float a = astep * float(vertexId) * (uTime + 10.) * .3;
	vec2 d = a * vec2(cos(a), sin(a)) / 100.;
	d.x *= uSize.y / uSize.x;
	gl_Position = vec4(d, 0, 1);
	v_color = vec4(1, 1, 1, 1);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;
in vec4 v_color;

void main(void)
{
	fragColor = v_color;
	fragColor.a = 1.0;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Points_3D_Lines() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform float(maxpoints) maxPoints; // count points (here 30000), not shown in uniforms pane
uniform mat4(camera:mvp) cameraMVP; // camera mvp (Model/View/proj), not shown with section name => hidden
uniform(lorenz:0) float(0:20:10) sigma;
uniform(lorenz:1) float(0:50:28) rho;
uniform(lorenz:2) float(0:5:2.66) beta;
uniform(precision) float(0.0001:0.01:0.00470) prec;
uniform(kernels:0) vec3(color:1,0,0) colKernel0;
uniform(kernels:1) vec3(color:0,0,1) colKernel1;
uniform(transition:0) vec3(color:1,1,1) colTransition;
uniform(transition:1) float(0:2:0.75) transitionStrength;
uniform(zoom) float(0:2:0.25) zoom;

@VERTEX POINTS(2000:30000:10000) DISPLAY(LINES)

layout(location = 0) in float vertexId;
out vec4 v_color;

void main()
{	
	vec3 last = vec3(0.00001) /* not zero */, new = last;
	
	for (float i=0.; i < maxPoints; i++)
	{
      	if (i > vertexId) break;
		last = new;
		
		// lorenz formula
		new.x = sigma * (last.y - last.x);
		new.y = last.x * (rho - last.z) - last.y;
		new.z = last.x * last.y - beta * last.z;
		
		new *= prec; // curve precision
		new += last; // accumulation
	}
	
  	// eachs two points => its the same point
	// for avoid path interruptions in LINES mode
	if (mod(vertexId, 2.) < 1.) 
	{
		new = last;
	}
	
	// kernels
	float r = rho - 1.;
	float k = sqrt(beta * r);	
	
	// kernel 0
	vec3 k0 = vec3(-k,-k,r); // kernel 0 pos
	float dk0 = length(new-k0); // distance of the current vertex to this kernel
			
	// kernel 1
	vec3 k1 = vec3(k,k,r); // kernel 1 pos
	float dk1 = length(new-k1); // distance of the current vertex to this kernel
	
	// kernels coloration according to distance between kernel 0 and 1
	if (dk0 < dk1)
	{
		float rk0 = dk0/(dk1*transitionStrength);;
		v_color = vec4(mix(colKernel0,colTransition, vec3(rk0)),1);
	}
	else
	{
		float rk1 = dk1/(dk0*transitionStrength);
		v_color = vec4(mix(colKernel1,colTransition, vec3(rk1)),1);
	}
	
	gl_Position = cameraMVP * vec4((new - vec3(0,0,r)).xzy * zoom,1);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;
in vec4 v_color;

void main()
{
	fragColor = v_color;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Points_3D_Triangles() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform mat4(camera:mvp) cam;
uniform float(time) time;

@VERTEX POINTS(2400) DISPLAY(TRIANGLES)

layout(location = 0) in float vertexId;
out vec4 v_color;

mat3 RotX(float a){return mat3(1.,0.,0.,0.,cos(a),-sin(a),0.,sin(a),cos(a));}
mat3 RotY(float a){return mat3(cos(a),0.,sin(a),0.,1.,0.,-sin(a),0.,cos(a));}
mat3 RotZ(float a){return mat3(cos(a),-sin(a),0.,sin(a),cos(a),0.,0.,0.,1.);}

void main()
{
	gl_PointSize = 3.0;
	
	vec3 p = vec3(0);
	
	float indexQuad = floor(vertexId / 6.);
	float index = mod(vertexId, 6.);
	float countSection = floor(6.);
	float indexCircle = floor(indexQuad / countSection);
	float astep = 3.14159 * 2.0 / countSection;
	float angle0 = indexQuad * astep;
	float angle1 = (indexQuad + 1.) * astep;
	float astepTorus = 3.14159 * 2.0 / floor(66.);
	float angleTorus = indexCircle * astepTorus;
	float radius = 4. * cos(angleTorus * 1.5 + time) * 2.;
	
	// triangle 1
	if (index == 0.) p = vec3(cos(angle0) * radius, 0., sin(angle0) * radius);
	if (index == 1.) p = vec3(cos(angle1) * radius, 0., sin(angle1) * radius);
	if (index == 2.) p = vec3(cos(angle1) * radius, 1., sin(angle1) * radius);
	
	// triangle 2
	if (index == 3.) p = vec3(cos(angle0) * radius, 0., sin(angle0) * radius);
	if (index == 4.) p = vec3(cos(angle1) * radius, 1., sin(angle1) * radius);
	if (index == 5.) p = vec3(cos(angle0) * radius, 1., sin(angle0) * radius);
	
  	float atten = p.x;
  
	p *= RotX(-angleTorus);
	
	p.z += 11. * cos(angleTorus);
	p.y += 11. * sin(angleTorus);
	
	gl_Position = cam * vec4(p, 1); // pos
  	p /= atten; // color
  
	v_color = vec4(normalize(p) * 0.5 + 0.5, 1);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;
in vec4 v_color;

void main()
{
	fragColor = v_color;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Mesh_Base() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform(mesh) vec3(color:0.2,0.8,0.5) color; // shape color

uniform(hidden) mat4(camera:mvp) cam;
uniform(hidden) mat4(camera:m) model;

uniform(normals:0) float(checkbox:false) useNormalAsColor; // dipslay normal
uniform(normals:1:useNormalAsColor==true) float(0.0:1.0:0.0) RatioNorCol; // display normal ratio

uniform(vertex:0) float(checkbox:false) useVertexColor; // use vertex color
uniform(vertex:1:useVertexColor==true) float(0.0:1.0:0.0) RatioVertCol; // vertex color ratio

uniform(light) float(checkbox:true) useLighing; // shader lighitng
uniform(light:useLighing==true:3) float(0:1:0.6) diffRatio; // diffuse ratio
uniform(light:useLighing==true:2) vec3(color:1.0,1.0,1.0) diffColor; // diffuse color
uniform(light:useLighing==true:4) float(0:1:0.6) speRatio; // specular ratio
uniform(light:useLighing==true:1) float(0:1:0.6) colRatio; // base color ration

uniform(bump mapping:0) float(checkbox:false) useNormalMap; // use bump mapping
uniform(bump mapping:1:useNormalMap==true) sampler2D(picture:choice) normalMap; // normal map

@VERTEX MESH(mesh/ico.obj)

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec3 a_bitangent;
layout(location = 4) in vec2 a_uv;
layout(location = 5) in vec4 a_color;

out vec3 v_position;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec2 v_uv;
out vec4 v_color;
out mat3 TBN;

mat3 getTBN(vec3 nor, vec3 tan, vec3 btan)
{
	vec3 N = normalize(vec3(model * vec4(nor, 0.0)));
	vec3 T = normalize(vec3(model * vec4(tan, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(vec3(model * vec4(btan, 0.0)));
	B = cross(N, T);
	return mat3(T, B, N);
}

void main()
{
	gl_PointSize = 3.;
	
	v_position = a_position;
	v_normal = a_normal;
	v_tangent = a_tangent;
	v_bitangent = a_bitangent;
	v_uv = a_uv;
	v_color = a_color;

	TBN = getTBN(v_normal, v_tangent, v_bitangent);
	
	gl_Position = cam * vec4(v_position, 1);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;

in vec3 v_position;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec2 v_uv;
in vec4 v_color;
in mat3 TBN;

// for get ray origin and direction
#include "space3d.glsl"

void main()
{
	vec3 n = v_normal;
	
	if (useNormalMap > 0.5)
	{
		vec3 nmap = texture(normalMap, v_uv).xyz * 2.0 - 1.0;
		n = normalize(TBN * nmap); 
	}
	
	vec3 ro = getRayOrigin();
	vec3 rd = getRayDirection();
	
	vec4 col = vec4(color, 1);
	
	if (useNormalAsColor > 0.5)
		col.rgb = (n * 0.5 + 0.5) * RatioNorCol;
	
	if (useVertexColor > 0.5)
		col += v_color * RatioVertCol;
	
	vec3 ld = -vec3(0.8,0.5,0.2);
	float diff = pow(dot(n, -ld) * .5 + .5,2.0);
    float spe = pow(max(dot(-rd, reflect(ld, n)), 0.0), 32.0);
		
	if (useLighing > 0.5)
		col.rgb = col.rgb * colRatio + 
			diff * diffColor * diffRatio + 
			spe * speRatio;
	
	fragColor = col;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Mesh_Tesselation() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform(shape) float(time) iTime; // shape animation time
uniform(shape) float(0:5:2) radius; // shape radius
uniform(shape) float(0:20:10) sinScale; // shape sin scale
uniform(shape) float(0:0.5:0.05) sinRadius; // shape sin radius
uniform(shape) vec3(color:0.2,0.8,0.5) color; // shape color

uniform(hidden) mat4(camera:mvp) cam;
uniform(hidden) mat4(camera:m) model;

uniform(tesselation:0) float(0:100:50:1) tessLevel;	// tesselation level
uniform(tesselation:1) float(-2.0:2.0:1.0) tessDisplace; // displacement scale
uniform(tesselation:2) float(0.0:10:1) normalFactor; // bump mapping

uniform(normals:0) float(checkbox:false) useNormalAsColor; // dipslay normal
uniform(normals:1:useNormalAsColor==true) float(0.0:1.0:0.0) RatioNorCol; // display normal ratio

uniform(vertex:0) float(checkbox:false) useVertexColor; // use vertex color
uniform(vertex:1:useVertexColor==true) float(0.0:1.0:0.0) RatioVertCol; // vertex color ratio

uniform(light) float(checkbox:true) useLighing; // shader lighitng
uniform(light:useLighing==true:3) float(0:1:0.6) diffRatio; // diffuse ratio
uniform(light:useLighing==true:2) vec3(color:1.0,1.0,1.0) diffColor; // diffuse color
uniform(light:useLighing==true:4) float(0:1:0.6) speRatio; // specular ratio
uniform(light:useLighing==true:1) float(0:1:0.6) colRatio; // base color ration

uniform(bump mapping:0) float(checkbox:false) useNormalMap; // use bump mapping
uniform(bump mapping:1:useNormalMap==true) sampler2D(picture:choice) normalMap; // normal map

@COMMON

vec4 displacement(vec3 p)
{
	float dist = sin( p.y * sinScale + iTime) * sinRadius;
	vec3 col = color * (dist / sinRadius * 0.5 + 0.5);
    return vec4(dist, col);
}

mat3 getTBN(vec3 nor, vec3 tan, vec3 btan)
{
	vec3 N = normalize(vec3(model * vec4(nor, 0.0)));
	vec3 T = normalize(vec3(model * vec4(tan, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(vec3(model * vec4(btan, 0.0)));
	B = cross(N, T);
	return mat3(T, B, N);
}

@VERTEX MESH(mesh/ico.obj)

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec3 a_bitangent;
layout(location = 4) in vec2 a_uv;
layout(location = 5) in vec4 a_color;

out vec3 v_position;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec2 v_uv;
out vec4 v_color;
out mat3 TBN;

void main()
{
	gl_PointSize = 3.;
	
	v_position = a_position;
	v_normal = a_normal;
	v_tangent = a_tangent;
	v_bitangent = a_bitangent;
	v_uv = a_uv;
	v_color = a_color;

	TBN = getTBN(v_normal, v_tangent, v_bitangent);
	
	gl_Position = cam * vec4(v_position, 1);
}

@TESSCONTROL

// define the number of CPs in the output patch
layout (vertices = 3) out;

// attributes of the input CPs
in vec3 v_position[];
in vec3 v_normal[];
in vec3 v_tangent[];
in vec3 v_bitangent[];
in vec2 v_uv[];
in vec4 v_color[];

out vec3 v_position_tc[];
out vec3 v_normal_tc[];
out vec3 v_tangent_tc[];
out vec3 v_bitangent_tc[];
out vec2 v_uv_tc[];
out vec4 v_color_tc[];

void main()
{
    // Set the control points of the output patch
    v_position_tc[gl_InvocationID] = v_position[gl_InvocationID];
	v_normal_tc[gl_InvocationID] = v_normal[gl_InvocationID];
	v_tangent_tc[gl_InvocationID] = v_tangent[gl_InvocationID];
	v_bitangent_tc[gl_InvocationID] = v_bitangent[gl_InvocationID];
	v_uv_tc[gl_InvocationID] = v_uv[gl_InvocationID];
	v_color_tc[gl_InvocationID] = v_color[gl_InvocationID];
	
	if (gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = tessLevel;
		gl_TessLevelOuter[1] = tessLevel;
		gl_TessLevelOuter[2] = tessLevel;
		gl_TessLevelOuter[3] = tessLevel;
		
		gl_TessLevelInner[0] = tessLevel;
		gl_TessLevelInner[1] = tessLevel;
	}
} 

@TESSEVAL

layout(triangles, equal_spacing, ccw) in;

in vec3 v_position_tc[];
in vec3 v_normal_tc[];
in vec3 v_tangent_tc[];
in vec3 v_bitangent_tc[];
in vec2 v_uv_tc[];
in vec4 v_color_tc[];

out vec3 v_position;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec2 v_uv;
out vec4 v_color;
out mat3 TBN;

#define interpolate(v0,v1,v2) gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
    v_position = interpolate(v_position_tc[0], v_position_tc[1], v_position_tc[2]);
	v_normal = interpolate(v_normal_tc[0], v_normal_tc[1], v_normal_tc[2]);
	v_tangent = interpolate(v_tangent_tc[0], v_tangent_tc[1], v_tangent_tc[2]);
	v_bitangent = interpolate(v_bitangent_tc[0], v_bitangent_tc[1], v_bitangent_tc[2]);
	v_uv = interpolate(v_uv_tc[0], v_uv_tc[1], v_uv_tc[2]);
	v_color = interpolate(v_color_tc[0], v_color_tc[1], v_color_tc[2]);

	v_normal = normalize(v_position);
	v_position = v_normal * radius;
	
	// Displace the vertex along the normal
	gl_Position = cam * vec4(v_position + v_normal * tessDisplace * displacement(v_position).x, 1.0);
	
	vec3 eps = vec3(0.01,0,0);
	float f = displacement(v_position).x;
	float fx = (f-displacement(v_position + eps.xyz).x)/eps.x;
	float fy = (f-displacement(v_position + eps.yxz).x)/eps.x;
	float fz = (f-displacement(v_position + eps.zyx).x)/eps.x;
	if (useNormalMap > 0.5)
		v_normal = vec3(0,0,1);
	v_normal = normalize(v_normal - vec3(fx,fy,fz) * normalFactor);
	
	TBN = getTBN(v_normal, v_tangent, v_bitangent);
} 

@FRAGMENT

layout(location = 0) out vec4 fragColor;

in vec3 v_position;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec2 v_uv;
in vec4 v_color;
in mat3 TBN;

// for get ray origin and direction
#include "space3d.glsl"

void main()
{
	vec3 n = v_normal;
	
	if (useNormalMap > 0.5)
	{
		vec3 nmap = texture(normalMap, v_uv).xyz * 2.0 - 1.0;
		n = normalize(TBN * nmap); 
	}
	
	vec3 ro = getRayOrigin();
	vec3 rd = getRayDirection();
	
	vec4 col = vec4(clamp(displacement(v_position).yzw,0.0,1.0),1);
	
	if (useNormalAsColor > 0.5)
		col.rgb = (n * 0.5 + 0.5) * RatioNorCol;
	
	if (useVertexColor > 0.5)
		col += v_color * RatioVertCol;
	
	vec3 ld = -vec3(0.8,0.5,0.2);
	float diff = pow(dot(n, -ld) * .5 + .5,2.0);
    float spe = pow(max(dot(-rd, reflect(ld, n)), 0.0), 32.0);
		
	if (useLighing > 0.5)
		col.rgb = col.rgb * colRatio + 
			diff * diffColor * diffRatio + 
			spe * speRatio;
	
	fragColor = col;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Mesh_Geometry() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform(hidden) float(usegeometry) useGeom;
uniform(hidden) mat4(camera:mvp) pucam;
uniform(Geometry) float(0:1:0.2) normalLength;
uniform(Geometry) float(0:1:0.1) normalBaseRatio;

@VERTEX MESH()

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
//layout(location = 2) in vec3 a_tangent;
//layout(location = 3) in vec3 a_bitangent;
//layout(location = 4) in vec2 a_uv;
//layout(location = 5) in vec4 a_color;

out vec3 v_position;
out vec3 v_normal;
//out vec3 v_tangent;
//out vec3 v_bitangent;
//out vec2 v_uv;
//out vec4 v_color;

out vec3 _position;
out vec3 _normal;
//out vec3 _tangent;
//out vec3 _bitangent;
//out vec2 _uv;
out vec4 _color;

void main()
{
	v_position = _position = a_position;
	v_normal = _normal = a_normal;
	//v_tangent = _tangent = a_tangent;
	//v_bitangent = _bitangent = a_bitangent;
	//v_uv = _uv = a_uv;
	//v_color = _color = a_color;

	_color = vec4(v_normal * 0.5 + 0.5, 1);
	
	gl_Position = pucam * vec4(v_position, 1);
}


@GEOMETRY

in vec3 v_position[];
in vec3 v_normal[];
//in vec3 v_tangent[];
//in vec3 v_bitangent[];
//in vec2 v_uv[];
//in vec4 v_color[];

//out vec3 _position;
//out vec3 _normal;
//out vec3 _tangent;
//out vec3 _bitangent;
//out vec2 _uv;
out vec4 _color;

@CONFIG_START Normals Wire (+2 verts)

layout(triangles) in; // input primitive(s) (must match the current render mode)
layout(line_strip, max_vertices=5) out; // output primitive(s)

void main()
{
	vec3 p = (v_position[0] + v_position[1] + v_position[2])/3.0;
	vec3 n = (v_normal[0] + v_normal[1] + v_normal[2])/3.0;
	_color = vec4(n * 0.5 + 0.5, 1);

	gl_Position = pucam * vec4(v_position[0], 1.0);
	EmitVertex();
	gl_Position = pucam * vec4(v_position[1], 1.0);
	EmitVertex();
	gl_Position = pucam * vec4(v_position[2], 1.0);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = pucam * vec4(p, 1.0);
	EmitVertex();
	gl_Position = pucam * vec4(p + n * normalLength, 1.0);
	EmitVertex();
	EndPrimitive();
}

@CONFIG_END

@CONFIG_START Normals Surf (+12 verts)

layout(triangles) in; // input primitive(s) (must match the current render mode)
layout(triangle_strip, max_vertices=15) out; // output primitive(s)

void main()
{
	vec3 p = (v_position[0] + v_position[1] + v_position[2])/3.0;
	vec3 n = (v_normal[0] + v_normal[1] + v_normal[2])/3.0;
	_color = vec4(n * 0.5 + 0.5, 1);
	
	// Face
	gl_Position = pucam * vec4(v_position[0], 1.0);
	EmitVertex();
	gl_Position = pucam * vec4(v_position[1], 1.0);
	EmitVertex();
	gl_Position = pucam * vec4(v_position[2], 1.0);
	EmitVertex();
	EndPrimitive();
	
	vec3 pn0 = mix(p, v_position[0], normalBaseRatio);
	vec3 pn1 = mix(p, v_position[1], normalBaseRatio);
	vec3 pn2 = mix(p, v_position[2], normalBaseRatio);
	
	//tri0
	_color = vec4(n * 0.5 + 0.25,1);
	gl_Position = pucam * vec4(pn0, 1.0); EmitVertex();
	gl_Position = pucam * vec4(pn1, 1.0); EmitVertex();
	_color = vec4(n * 0.5 + 0.5, 1);
	gl_Position = pucam * vec4(p + n * normalLength, 1.0); EmitVertex();
	EmitVertex();
	EndPrimitive();
	
	//tri1
	_color = vec4(n * 0.5 + 0.25,1);
	gl_Position = pucam * vec4(pn1, 1.0); EmitVertex();
	gl_Position = pucam * vec4(pn2, 1.0); EmitVertex();
	_color = vec4(n * 0.5 + 0.5, 1);
	gl_Position = pucam * vec4(p + n * normalLength, 1.0); EmitVertex();
	EmitVertex();
	EndPrimitive();
	
	//tri2
	_color = vec4(n * 0.5 + 0.25,1);
	gl_Position = pucam * vec4(pn2, 1.0); EmitVertex();
	gl_Position = pucam * vec4(pn0, 1.0); EmitVertex();
	_color = vec4(n * 0.5 + 0.5, 1);
	gl_Position = pucam * vec4(p + n * normalLength, 1.0); EmitVertex();
	EmitVertex();
	EndPrimitive();
}

@CONFIG_END

@FRAGMENT

layout(location = 0) out vec4 fragColor;

//in vec3 _position;
//in vec3 _normal;
in vec4 _color;

void main()
{
	fragColor = _color;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Mesh_Capture() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform(shape) float(time) iTime; // shape animation time
uniform(shape) float(0:5:2) radius; // shape radius
uniform(shape) float(0:20:10) sinScale; // shape sin scale
uniform(shape) float(0:0.5:0.05) sinRadius; // shape sin radius
uniform(shape) vec3(color:0.2,0.8,0.5) color; // shape color

uniform(hidden) mat4(camera:mvp) cam;
uniform(hidden) mat4(camera:m) model;

uniform(tesselation:0) float(0:100:50:1) tessLevel;	// tesselation level
uniform(tesselation:1) float(-2.0:2.0:1.0) tessDisplace; // displacement scale
uniform(tesselation:2) float(0.0:10:1) normalFactor; // bump mapping

uniform(normals:0) float(checkbox:false) useNormalAsColor; // dipslay normal
uniform(normals:1:useNormalAsColor==true) float(0.0:1.0:0.0) RatioNorCol; // display normal ratio

uniform(light) float(checkbox:true) useLighing; // shader lighitng
uniform(light:useLighing==true:3) float(0:1:0.6) diffRatio; // diffuse ratio
uniform(light:useLighing==true:2) vec3(color:1.0,1.0,1.0) diffColor; // diffuse color
uniform(light:useLighing==true:4) float(0:1:0.6) speRatio; // specular ratio
uniform(light:useLighing==true:1) float(0:1:0.6) colRatio; // base color ration

uniform(bump mapping:0) float(checkbox:false) useNormalMap; // use bump mapping
uniform(bump mapping:1:useNormalMap==true) sampler2D(picture:choice) normalMap; // normal map

@COMMON

vec4 displacement(vec3 p)
{
	float dist = sin(p.y * sinScale + iTime) * sinRadius;
	vec3 col = color * (dist / sinRadius * 0.5 + 0.5);
    return vec4(dist, col);
}

mat3 getTBN(vec3 nor, vec3 tan, vec3 btan)
{
	vec3 N = normalize(vec3(model * vec4(nor, 0.0)));
	vec3 T = normalize(vec3(model * vec4(tan, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(vec3(model * vec4(btan, 0.0)));
	B = cross(N, T);
	return mat3(T, B, N);
}

@VERTEX MESH(mesh/ico.obj)

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec3 a_bitangent;
layout(location = 4) in vec2 a_uv;
layout(location = 5) in vec4 a_color;

out vec3 v_position;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec2 v_uv;
out vec4 v_color;
out mat3 TBN;

// for capture, fill datas, then in menu trasnofrm feedback
// press add, select the buffer size and click on capture
// check the captured size, if its the same size, 
// you must increase the buffer size
// then export to a file
layout(xfb_buffer = 0, xfb_stride = 48) out Data
{
	layout(xfb_offset = 0) vec3 pos;
	layout(xfb_offset = 12) vec3 nor;
	layout(xfb_offset = 24) vec2 uv;
	layout(xfb_offset = 32) vec4 col;
} datas;

void main()
{
	gl_PointSize = 3.;
	
	v_position = a_position;
	v_normal = a_normal;
	v_tangent = a_tangent;
	v_bitangent = a_bitangent;
	v_uv = a_uv;
	v_color = a_color;
	
	TBN = getTBN(v_normal, v_tangent, v_bitangent);
	
	gl_Position = cam * vec4(v_position, 1);
	
	v_color = vec4(clamp(displacement(v_position).yzw, 0.0, 1.0), 1.0);

	datas.pos = v_position;
	datas.nor = v_normal;
	datas.uv = v_uv;
	datas.col = v_color;
}

@TESSCONTROL

// define the number of CPs in the output patch
layout (vertices = 3) out;

// attributes of the input CPs
in vec3 v_position[];
in vec3 v_normal[];
in vec3 v_tangent[];
in vec3 v_bitangent[];
in vec2 v_uv[];
in vec4 v_color[];

out vec3 v_position_tc[];
out vec3 v_normal_tc[];
out vec3 v_tangent_tc[];
out vec3 v_bitangent_tc[];
out vec2 v_uv_tc[];
out vec4 v_color_tc[];

void main()
{
    // Set the control points of the output patch
    v_position_tc[gl_InvocationID] = v_position[gl_InvocationID];
	v_normal_tc[gl_InvocationID] = v_normal[gl_InvocationID];
	v_tangent_tc[gl_InvocationID] = v_tangent[gl_InvocationID];
	v_bitangent_tc[gl_InvocationID] = v_bitangent[gl_InvocationID];
	v_uv_tc[gl_InvocationID] = v_uv[gl_InvocationID];
	v_color_tc[gl_InvocationID] = v_color[gl_InvocationID];
	
	if (gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = tessLevel;
		gl_TessLevelOuter[1] = tessLevel;
		gl_TessLevelOuter[2] = tessLevel;
		gl_TessLevelOuter[3] = tessLevel;
		
		gl_TessLevelInner[0] = tessLevel;
		gl_TessLevelInner[1] = tessLevel;
	}
} 

@TESSEVAL

layout(triangles, equal_spacing, ccw) in;

in vec3 v_position_tc[];
in vec3 v_normal_tc[];
in vec3 v_tangent_tc[];
in vec3 v_bitangent_tc[];
in vec2 v_uv_tc[];
in vec4 v_color_tc[];

out vec3 v_position;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec2 v_uv;
out vec4 v_color;
out mat3 TBN;

#define interpolate(v0,v1,v2) gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2

// for capture, fill datas, then in menu trasnofrm feedback
// press add, select the buffer size and click on capture
// check the captured size, if its the same size, 
// you must increase the buffer size
// then export to a file
layout(xfb_buffer = 0, xfb_stride = 48) out Data
{
	layout(xfb_offset = 0) vec3 pos;
	layout(xfb_offset = 12) vec3 nor;
	layout(xfb_offset = 24) vec2 uv;
	layout(xfb_offset = 32) vec4 col;
} datas;

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
    v_position = interpolate(v_position_tc[0], v_position_tc[1], v_position_tc[2]);
	v_normal = interpolate(v_normal_tc[0], v_normal_tc[1], v_normal_tc[2]);
	v_tangent = interpolate(v_tangent_tc[0], v_tangent_tc[1], v_tangent_tc[2]);
	v_bitangent = interpolate(v_bitangent_tc[0], v_bitangent_tc[1], v_bitangent_tc[2]);
	v_uv = interpolate(v_uv_tc[0], v_uv_tc[1], v_uv_tc[2]);
	v_color = interpolate(v_color_tc[0], v_color_tc[1], v_color_tc[2]);

	v_normal = normalize(v_position);
	v_position = v_normal * radius;
	
	// Displace the vertex along the normal
	v_position = v_position + v_normal * tessDisplace * displacement(v_position).x;
	gl_Position = cam * vec4(v_position, 1.0);
	
	vec3 eps = vec3(0.01,0,0);
	float f = displacement(v_position).x;
	float fx = (f-displacement(v_position + eps.xyz).x)/eps.x;
	float fy = (f-displacement(v_position + eps.yxz).x)/eps.x;
	float fz = (f-displacement(v_position + eps.zyx).x)/eps.x;
	if (useNormalMap > 0.5)
		v_normal = vec3(0,0,1);
	v_normal = normalize(v_normal - vec3(fx,fy,fz) * normalFactor);
	
	TBN = getTBN(v_normal, v_tangent, v_bitangent);
	
	v_color = vec4(clamp(displacement(v_position).yzw, 0.0, 1.0), 1.0);

	datas.pos = v_position;
	datas.nor = v_normal;
	datas.uv = v_uv;
	datas.col = v_color;
} 

@FRAGMENT

layout(location = 0) out vec4 fragColor;

in vec3 v_position;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec2 v_uv;
in vec4 v_color;
in mat3 TBN;

// for get ray origin and direction
#include "space3d.glsl"

void main()
{
	vec3 n = v_normal;
	
	if (useNormalMap > 0.5)
	{
		vec3 nmap = texture(normalMap, v_uv).xyz * 2.0 - 1.0;
		n = normalize(TBN * nmap); 
	}
	
	vec3 ro = getRayOrigin();
	vec3 rd = getRayDirection();
	
	vec4 col = v_color;
	
	if (useNormalAsColor > 0.5)
		col.rgb = (n * 0.5 + 0.5) * RatioNorCol;
	
	vec3 ld = -vec3(0.8,0.5,0.2);
	float diff = pow(dot(n, -ld) * .5 + .5,2.0);
    float spe = pow(max(dot(-rd, reflect(ld, n)), 0.0), 32.0);
		
	if (useLighing > 0.5)
		col.rgb = col.rgb * colRatio + 
			diff * diffColor * diffRatio + 
			spe * speRatio;
	
	fragColor = col;
}
)";
    return infos;
}

// web sites shaders

ShaderInfos CodeGenerator::Get_Shader_ShaderToy_Image() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER 

WRAP(clamp) 
FILTER(linear) 
MIPMAP(false) 
//COUNT(1 to 8)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform float(time) iTime;
uniform int(frame) iFrame;
uniform float(deltatime) iTimeDelta;
uniform vec3(buffer:target=0) iResolution;
uniform vec4(mouse:2pos_2click) iMouse;
uniform vec4(date) iDate;

@VERTEX

layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;

void mainImage( out vec4 fragColor, in vec2 fragCoord)
{
	vec2 uv = fragCoord / iResolution.xy;
	vec4 color = vec4(0);
	color.rg += cos(uv + iTime) * 0.5 + 0.5;
	color.gb += sin(uv + iTime) * 0.5 + 0.5;
	fragColor = color;
}

void main(void)
{
	mainImage(fragColor, gl_FragCoord.xy);
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_ShaderToy_Buffer() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER

WRAP(clamp) 
FILTER(linear) 
MIPMAP(false) 
//COUNT(1 to 8)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform float(time) iTime;
uniform int(frame) iFrame;
uniform float(deltatime) iTimeDelta;
uniform vec3(buffer:target=0) iResolution;
uniform vec4(mouse:2pos_2click) iMouse;
uniform vec4(date) iDate;

@VERTEX

layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;

void mainImage( out vec4 fragColor, in vec2 fragCoord)
{
	vec2 uv = fragCoord / iResolution.xy;
	vec4 color = vec4(0);
	color.rg += cos(uv + iTime) * 0.5 + 0.5;
	color.gb += sin(uv + iTime) * 0.5 + 0.5;
	fragColor = color;
}

void main(void)
{
	mainImage(fragColor, gl_FragCoord.xy);
	fragColor.a = 1.0;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_GlslSandbox_Base() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER

FORMAT(byte) 
//COUNT(1 to 8)
//WRAP(clamp or repeat or mirror)
//FILTER(linear or nearest)
//MIPMAP(false or true)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform float(time) time;
uniform vec2(buffer:target=0) resolution;
uniform vec2(mouse:normalized) mouse;

@VERTEX

layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@FRAGMENT

// UnComment theses line in webgl ///////////////////////////////
/*
#extension GL_OES_standard_derivatives : enable
uniform float time;
uniform vec2 resolution;
uniform vec2 mouse;
*/
//#extension GL_OES_standard_derivatives : enable
// Remove this in WebGl2 //////////////////////////////////////
layout(location = 0) out vec4 fragColor;
// Dont forgot to rename fragColor by gl_FragColor in webgl ///
///////////////////////////////////////////////////////////////
void main(void) 
{
	vec2 position = (gl_FragCoord.xy / resolution.xy) + mouse / 4.0;
	float color = 0.0;
	color += sin(position.x * cos(time / 15.0) * 80.0) + cos(position.y * cos(time / 15.0) * 10.0);
	color += sin(position.y * sin(time / 10.0) * 40.0) + cos(position.x * sin(time / 25.0) * 40.0);
	color += sin(position.x * sin(time / 5.0) * 10.0) + sin(position.y * sin(time / 35.0) * 80.0);
	color *= sin(time / 10.0) * 0.5;
	fragColor = vec4(vec3(color, color * 0.5, sin(color + time / 3.0) * 0.75), 1.0);
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_GlslSandbox_MultiPass() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER

FORMAT(byte) 
//COUNT(1 to 8)
//WRAP(clamp or repeat or mirror)
//FILTER(linear or nearest)
//MIPMAP(false or true)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform float(time) time;
uniform vec2(buffer:target=0) resolution;
uniform vec2(mouse:normalized) mouse;
uniform sampler2D(buffer:target=0) backBuffer;

@VERTEX

layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@FRAGMENT

// UnComment these line in webgl ///////////////////////////////
/*
#extension GL_OES_standard_derivatives : enable
uniform float time;
uniform vec2 resolution;
uniform vec2 mouse;
uniform sampler2D backBufferr;
*/
// Remove this in WebGl2 //////////////////////////////////////
layout(location = 0) out vec4 fragColor;
// Dont forgot to rename fragColor by gl_FragColor in webgl ///
///////////////////////////////////////////////////////////////
void main()
{
	float halfpi = atan(-1.0)*-2.0;
	vec2 s = resolution.xy;
	vec2 g = gl_FragCoord.xy;
	vec4 h = texture(backBuffer, g / s);
	g = (gl_FragCoord.xy * 2. - s) / s.y*1.3;
	vec2
		k = vec2(halfpi, 0) + mod(time, 4.0 * halfpi),
		a = g - sin(k),
		b = g - sin(2.09 + k),
		c = g - sin(4.18 + k);
	fragColor = (0.02 / dot(a, a) + 0.02 / dot(b, b) + 0.02 / dot(c, c)) * 0.04 + h * 0.97 + step(h, vec4(.8, .2, .5, 1)) * 0.01;
	fragColor.a = 1.0;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_GlslSandbox_SurfacePosition() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER

FORMAT(byte) 
//COUNT(1 to 8)
//WRAP(clamp or repeat or mirror)
//FILTER(linear or nearest)
//MIPMAP(false or true)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform float(time) time;
uniform vec2(buffer:target=0) resolution;
uniform vec2(mouse:normalized) mouse;

@VERTEX

out vec2 surfacePosition;
layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
	surfacePosition = a_position;
}

@FRAGMENT

// Rename out by varyinge in webgl ////////////////////////////
in vec2 surfacePosition;
// UnComment this line in webgl ///////////////////////////////
/*
#extension GL_OES_standard_derivatives : enable
uniform float time;
uniform vec2 resolution;
uniform vec2 mouse;
*/
//#extension GL_OES_standard_derivatives : enable
// Remove this in WebGl2 //////////////////////////////////////
layout(location = 0) out vec4 fragColor;
// Dont forgot to rename fragColor by gl_FragColor in webgl ///
///////////////////////////////////////////////////////////////
void main(void) 
{
	vec2 position = surfacePosition + mouse / 4.0;
	float color = 0.0;
	color += sin(position.x * cos(time / 15.0) * 80.0) + cos(position.y * cos(time / 15.0) * 10.0);
	color += sin(position.y * sin(time / 10.0) * 40.0) + cos(position.x * sin(time / 25.0) * 40.0);
	color += sin(position.x * sin(time / 5.0) * 10.0) + sin(position.y * sin(time / 35.0) * 80.0);
	color *= sin(time / 10.0) * 0.5;
	fragColor = vec4(vec3(color, color * 0.5, sin(color + time / 3.0) * 0.75), 1.0);
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_VertexShaderArt_Base() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform float(time) time;
uniform vec2(buffer:target=0) resolution;
uniform vec2(mouse:normalized) mouse;

@VERTEX POINTS(0:20000:20000)

layout(location = 0) in float vertexId;
out vec4 v_color;

#define PI radians(180.)
#define NUM_SEGMENTS 2.0
#define NUM_POINTS (NUM_SEGMENTS * 2.0)
#define STEP 1.0

void main()
{
	float point = mod(floor(vertexId / 2.0) + mod(vertexId, 2.0) * STEP, NUM_SEGMENTS);
	float count = floor(vertexId / NUM_POINTS);
	float offset = count * sin(time * 0.01) + 5.0;
	float angle = point * PI * 2.0 / NUM_SEGMENTS + offset;
	float radius = pow(count * 0.00014, 1.0);
	float c = cos(angle + time) * radius;
	float s = sin(angle + time) * radius;
	float orbitAngle = pow(count * 0.025, 0.8);
	float innerRadius = pow(count * 0.0005, 1.2);
	float oC = cos(orbitAngle + count * 0.0001) * innerRadius;
	float oS = sin(orbitAngle + count * 0.0001) * innerRadius;
	vec2 aspect = vec2(1, resolution.x / resolution.y);
	vec2 xy = vec2(
		oC + c,
		oS + s);
	gl_Position = vec4(xy * aspect + mouse * 0.1, 0, 1);
	float b = 1.0 - pow(sin(count * 0.4) * 0.5 + 0.5, 10.0);
	b = 0.0; mix(0.0, 0.7, b);
	v_color = vec4(b, b, b, 1);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;
in vec4 v_color;

void main(void)
{
	fragColor = v_color;
	fragColor.a = 1.0;
}
)";
    return infos;
}

// Help

ShaderInfos CodeGenerator::Get_Shader_Uniforms_Help() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER 

//FORMAT(float or byte)
//COUNT(1 to 8)
//WRAP(clamp or repeat or mirror)
//FILTER(linear or nearest)
//MIPMAP(false or true)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform(time:0) float(time:true) uEnabledTime; // enabled by default
uniform(time:1) float(time:false) uDisabledTime; // disabled by default
uniform(time:2) float(time:true:3.14159) uPeriodTime; // period of 3.14159 time

uniform(frame) int(frame) uFrame;
uniform(deltatime) float(deltatime) uDeltaTime; // last render time
uniform(buffer) vec2(buffer:target=0) uBufferSize;

uniform(Sliders Float) float(-2:2:0:0.1) uSliderF;
uniform(Sliders Float) vec2(-2:2:0:0.1) uSliderV2;
uniform(Sliders Float) vec3(-2:2:0:0.1) uSliderV3;
uniform(Sliders Float) vec4(-2:2:0:0.1) uSliderV4;

uniform(Sliders Int:-1) int(-100:100:0) uSliderI;
uniform(Sliders Int) ivec2(-100:100:0) uSliderIV2;
uniform(Sliders Int) ivec3(-100:100:0) uSliderIV3;
uniform(Sliders Int) ivec4(-100:100:0) uSliderIV4;

uniform(Sliders UInt) uint(0:200:100) uSliderU;
uniform(Sliders UInt) uvec2(0:200:100) uSliderUV2;
uniform(Sliders UInt) uvec3(0:200:100) uSliderUV3;
uniform(Sliders UInt) uvec4(0:200:100) uSliderUV4;

uniform(checkbox float:0) float(checkbox:true) uCheckF;
uniform(checkbox float:1) vec2(checkbox:false,true) uCheckV2;
uniform(checkbox float:2) vec3(checkbox:false,true,false) uCheckV3;
uniform(checkbox float:3) vec4(checkbox:true,true,false,true) uCheckV4;

uniform(checkbox bool:0) bool(checkbox:true) uCheckB;
uniform(checkbox bool:1) bvec2(checkbox:false,true) uCheckBV2;
uniform(checkbox bool:2) bvec3(checkbox:false,true,false) uCheckBV3;
uniform(checkbox bool:3) bvec4(checkbox:true,true,false,true) uCheckBV4;

uniform(radio float:1) vec2(radio:1) uRadioV2;
uniform(radio float:2) vec3(radio:2) uRadioV3;
uniform(radio float:3) vec4(radio:0) uRadioV4;

uniform(radio bool:1) bvec2(radio:0) uRadioBV2;
uniform(radio bool:2) bvec3(radio:1) uRadioBV3;
uniform(radio bool:3) bvec4(radio:2) uRadioBV4;

uniform(color) vec3(color:0.8,0.5,0.2) uColorV3;
uniform(color) vec4(color:0.2,0.4,0.5,0.8) uColorV4;

uniform(date) vec4(date) uDate;

uniform(combobox) int(combobox:option 1,option 2, option 3:1) uCombobox;

uniform(button float) float(button: press me) uButtonF; // when the button is pressed\b the value of uButtonF is 1.0 else 0.0
uniform(button float) vec2(button: press me 1, press me 2) uButtonV2;
uniform(button float) vec3(button: press me 1, press me 2, press me 3) uButtonV3;
uniform(button float) vec4(button: press me 1, press me 2, press me 3, press me 4) uButtonV4;

uniform(button bool) bool(button: press me) uButtonB; // when the button is pressed\b the value of uButtonB is true else false
uniform(button bool) bvec2(button: press me 1, press me 2) uButtonBV2;
uniform(button bool) bvec3(button: press me 1, press me 2, press me 3) uButtonBV3;
uniform(button bool) bvec4(button: press me 1, press me 2, press me 3, press me 4) uButtonBV4;

uniform(gamepad) vec3(gamepad:leftthumb) uLThumb;
uniform(gamepad) vec3(gamepad:rightthumb) uRThumb;
uniform(gamepad) float(gamepad:lefttrigger) uLTrigger;
uniform(gamepad) float(gamepad:righttrigger) uRTrigger;
uniform(gamepad) vec4(gamepad:cross) uCross;
uniform(gamepad) vec4(gamepad:mainbuttons) uMainButtons;

// normally you dont need to use theses three uniforms
// you just need to include "GizmoCulling.glsl" in your code, and use the internal function in this include
uniform(culling:0) text(normaly you dont need to use theses three uniforms\nyou just need to include "GizmoCulling.glsl" in your code,\nand use the internal function of this include) uText;
uniform(culling:1) int(cullingtype) uCullingType; // 0 => cube, 1 => sphere
uniform(culling:2) float(useculling) uUseCulling; // true if culling is enabled
uniform(culling:3) mat4(culling) uCulling; // gizmo matrix for culling primitive

uniform(gizmo) mat4(gizmo) uGizmo;

@VERTEX

layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;

void main(void)
{
	vec2 uv = gl_FragCoord.xy / uBufferSize;

	fragColor = sin(uv.xyxy * vec4(3,5,10,1) + uEnabledTime) * 0.5 + 0.5;
	fragColor.a = 1.0;
}
)";
    return infos;
}

// Feature samples

ShaderInfos CodeGenerator::Get_Shader_Demo_Gizmo() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER

//FORMAT(float or byte)
//COUNT(1 to 8)
//WRAP(clamp or repeat or mirror)
//FILTER(linear or nearest)
//MIPMAP(false or true)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform float(time) iTime;
uniform vec4(mouse:2pos_2click) iMouse;
uniform vec4(date) iDate;
uniform int(frame) iFrame;
uniform float(deltatime) iTimeDelta;

uniform(sdf) mat4(gizmo:trans=.5,.6,.8) sphereOrigin;
uniform(sdf) float(.1:10:3) sphereRadius;
uniform(sdf) mat4(gizmo) cubeOrigin;
uniform(sdf) float(.1:10:2.3) cubeSize;
uniform(sdf) float(.1:1:.2) cubeCorners;

uniform(raymarcher) int(1:200:100) countSteps;
uniform(raymarcher) float(0:1:.6) stepScale;
uniform(raymarcher) float(0:200:100) maxDistance;

uniform(light) float(0:.01:.001) normalPrec;
uniform(light) mat4(gizmo:trans=1,2.5,3) light0;
uniform(light) vec3(color:1,1,1) light0Col;
uniform(light) mat4(gizmo:trans=5,3.5,.5) light1;
uniform(light) vec3(color:1,1,1) light1Col;

uniform(color) vec3(color:.43,.38,.8) matColor0;
uniform(color) vec3(color:.25,.5,.1) matColor1;

@VERTEX

layout(location = 0) in vec2 a_position; // Current vertex position

out vec2 uv;

void main()
{
	uv = a_position * 2.0 - 1.0;
	gl_Position = vec4(uv, 0.0, 1.0);
}

@FRAGMENT

in vec2 uv;
layout(location = 0) out vec4 fragColor;

#include "space3d.glsl"

vec2 minObj(vec2 a, vec2 b)
{
	if (a.x < b.x)
		return a;
	return b;
}

vec2 maxObj(vec2 a, vec2 b)
{
	if (a.x > b.x)
		return a;
	return b;
}

vec2 GetSDF(vec3 p)
{
	float dSphere = length(p - sphereOrigin[3].xyz) - sphereRadius;
	vec2 resSDF = vec2(dSphere, 0);

	vec3 q = (p - cubeOrigin[3].xyz) * mat3(cubeOrigin);
	float dCube = length(max(abs(q.xyz) - cubeSize, 0.0)) - cubeCorners;
	vec2 resCube = vec2(dCube, 1);

	return maxObj(-resSDF, resCube);
}

float march(in vec3 ro, in vec3 rd)
{
	float s = 1.;
	float d = 0.;
	for (int i = 0; i < countSteps; i++)
	{
		if (d*d / s > 1e5 || d > maxDistance) break;
		s = GetSDF(ro + rd * d).x;
		d += s * stepScale;
	}
	return d;
}

vec3 getNor(vec3 p, float k)
{
	vec3 eps = vec3(k, 0., 0.);
	vec3 nor = vec3(
		GetSDF(p + eps.xyy).x - GetSDF(p - eps.xyy).x,
		GetSDF(p + eps.yxy).x - GetSDF(p - eps.yxy).x,
		GetSDF(p + eps.yyx).x - GetSDF(p - eps.yyx).x);
	return normalize(nor);
}

float GetSha(in vec3 ro, in vec3 rd, in float hn)
{
	float res = 1.0;
	float t = 0.0005;
	float h = 1.0;
	for (int i = 0; i < 40; i++)
	{
		h = GetSDF(ro + rd * t).x;
		res = min(res, hn*h / t);
		t += clamp(h, 0.02, 2.0);
	}
	return clamp(res, 0.0, 1.0);
}

float getAO(in vec3 p, in vec3 nor)
{
	float occ = 0.0;
	float sca = 1.0;
	for (int i = 0; i < 5; i++)
	{
		float hr = 0.01 + 0.12 * float(i) / 4.0;
		vec3 aopos = nor * hr + p;
		float dd = GetSDF(aopos).x;
		occ += -(dd - hr)*sca;
		sca *= 0.95;
	}
	return clamp(1.0 - 3.0*occ, 0.0, 1.0);
}

vec3 light(in vec3 lightdir, in vec3 lightcol, in vec3 tex, in vec3 norm, in vec3 camdir)
{
	float cosa = pow(0.5 + 0.5*dot(norm, -lightdir), 2.0);
	float cosr = max(dot(-camdir, reflect(lightdir, norm)), -0.0);

	float diffuse = cosa;
	float phong = pow(cosr, 32.0);

	return lightcol * 1.5 * (tex * diffuse + phong);
}

vec3 material(in vec3 pos, in vec3 camdir)
{
	vec3 norm = getNor(pos, normalPrec);

	vec3 d1 = -normalize(light0[3].xyz);
	vec3 d2 = -normalize(light1[3].xyz);

	vec3 tex = vec3(1.0);
	float sha = 1.0;
	float ao = 1.0;

	float mat = GetSDF(pos).y;
	if (mat < 1.)
		tex = matColor0;
	else if (mat < 2.)
		tex = matColor1;

	sha = 0.5 + 0.5 * GetSha(pos, norm, 2.0);
	ao = getAO(pos, norm);

	vec3 l1 = light(d1, light0Col, tex, norm, camdir);
	vec3 l2 = light(d2, light1Col, tex, norm, camdir);

	vec3 amb = vec3(0.05);

	return amb * ao + (l1 + l2) * sha;
}

void mainImage(inout vec4 fragColor, in vec2 fragCoord)
{
	float t = iTime;

	vec3 ro = getRayOrigin();
	vec3 rd = getRayDirection();

	vec3 col = vec3(0.0);

	float d = march(ro, rd);

	CalcDepth(rd, d);

	if (d < maxDistance)
	{
		col = material(ro + rd * d, rd);
		fragColor = vec4(sqrt(col*col*1.2), 1.0);
	}
	else
	{
		discard;
	}
}

void main(void)
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	mainImage(color, gl_FragCoord.xy);
	color.a = 1.0;
	fragColor = color;
	fragColor.a = 1.0;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Demo_Space3D() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER

//FORMAT(float or byte)
//COUNT(1 to 8)
//WRAP(clamp or repeat or mirror)
//FILTER(linear or nearest)
//MIPMAP(false or true)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)\n\n";

@UNIFORMS

uniform float(time) iTime;

@VERTEX

layout(location = 0) in vec2 a_position; // Current vertex position

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@NOTE

for show the sapce3d grid, go in menu Settings/show 3d space

@FRAGMENT

layout(location = 0) out vec4 fragColor;

#include "space3d.glsl"

mat3 rx(float a){return mat3(1.,0.,0.,0.,cos(a),-sin(a),0.,sin(a),cos(a));}
mat3 ry(float a){return mat3(cos(a),0.,sin(a),0.,1.,0.,-sin(a),0.,cos(a));}
mat3 rz(float a){return mat3(cos(a),-sin(a),0.,sin(a),cos(a),0.,0.,0.,1.);}

@UNIFORMS

uniform(sdf) float(0:1:0.1) fillet; // fillet of the cube

uniform(sdf cut:0) float(checkbox:false) useCutting;
uniform(sdf cut:1:useCutting==true) float(-5:5:0) cutting;

uniform(sdf shell:2) float(checkbox:false) useShell;
uniform(sdf shell:3:useShell==true) float(0.001:0.5:0.05) shellThickNess;  // thickness of the cube

@FRAGMENT

float sdf(vec3 p)
{
	float cuttingPlane = p.z + cutting;
	p *= rx(iTime) * ry(iTime) * rz(iTime);
	float cube = length(max(abs(p)-1.,0.0))-fillet;
	if (useShell > 0.5)
		cube = abs(cube) - shellThickNess;
	if (useCutting > 0.5)
	{
		return max(cuttingPlane, cube);
	}
	return cube;
}

vec3 nor( in vec3 p, float prec )
{
	vec3 eps = vec3( prec, 0., 0. );
	vec3 nor = vec3(
	    sdf(p+eps.xyy) - sdf(p-eps.xyy),
	    sdf(p+eps.yxy) - sdf(p-eps.yxy),
	    sdf(p+eps.yyx) - sdf(p-eps.yyx) );
	return normalize(nor);
}

@UNIFORMS

uniform(color) vec3(color:0.2,0.4,0.8) matColor;

@FRAGMENT

void main(void)
{
	vec3 ro = getRayOrigin();
	vec3 rd = getRayDirection();
	
	vec3 col = vec3(0);
	
	float md = 50.;
    float s = 1.;
    float d = 0.;
	
	const float iter = 250.;
    for(float i=0.;i<iter;i++)
    {      
        if (d*d/s>1e5||d>md) break;
        s = sdf(ro+rd*d).x;
		d += s * 0.5;
    }
    
	CalcDepth(rd, d);
	
	if (d<md)
	{
        vec3 p = ro + rd * d;
		vec3 n = nor(p, 0.01);
		vec3 refl = reflect(rd,n);
			
		vec3 ld = normalize(vec3(ro.x, 5,ro.z));
		
		float amb = 0.6;
		float diff = clamp( dot( n, ld ), 0.0, 1.0 );
		float fre = pow( clamp( 1. + dot(n,rd),0.0,1.0), 4. );
		float spe = pow(clamp( dot( refl, ld ), 0.0, 1.0 ),16.);
			
		col = vec3(0);
		col += amb * matColor;
		col += diff * 0.6;
		col += spe * 0.8;
	}
	else
	{
		discard;
	}
	
	fragColor = vec4(col,1);
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Demo_Gizmo_Culling() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER 

//FORMAT(float or byte)
//COUNT(1 to 8)
//WRAP(clamp or repeat or mirror)
//FILTER(linear or nearest)
//MIPMAP(false or true)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform(Ray Marching) float(50:300:50) rmFar;
uniform(Ray Marching) float(0.01:1.0:0.6) rmPrec;
uniform(Ray Marching) int(1:300:150) rmIters;

uniform(sdf) mat4(gizmo:trans=1.5,0.5,1.5) sphereOrigin;
uniform(sdf) mat4(gizmo) cubeOrigin;
uniform(sdf) float(0.1:1.0:0.2) cubeCorners;

uniform(light) float(0.0:0.01 : 0.001) normalPrec;
uniform(light) vec3(color:1.0, 1.0, 1.0) light0Col;

uniform(material) vec3(color:0.43, 0.38, 0.8) matColor0;
uniform(material) vec3(color:0.25, 0.5, 0.1) matColor1;

@NOTE

you msut use the gizmo button in menu bar for show gizmo widgets
after that activate the culling in the menu gizmo/use Culling

@VERTEX

layout(location = 0) in vec2 a_position; // Current vertex position

out vec2 uv;

void main()
{
	uv = a_position * 2.0 - 1.0;
	gl_Position = vec4(uv, 0.0, 1.0);
}

@FRAGMENT

in vec2 uv;
layout(location = 0) out vec4 fragColor;

#include "space3d.glsl"
#include "culling.glsl"

vec2 minObj(vec2 a, vec2 b){if (a.x < b.x)return a;return b;}
vec2 maxObj(vec2 a, vec2 b){if (a.x > b.x)return a;return b;}

vec2 GetSDF(vec3 p)
{
	vec2 sphere = vec2(length(p - sphereOrigin[3].xyz) - 0.5, 0.);
	p = (p - cubeOrigin[3].xyz) * mat3(cubeOrigin);
	vec2 box = vec2(length(max(abs(p) - 0.3, 0.0)) - cubeCorners,1.);
	return minObj(sphere, box);
}

vec3 getNor(vec3 p, float k)
{
	vec3 eps = vec3(k, 0., 0.);
	vec3 nor = vec3(
		GetSDF(p + eps.xyy).x - GetSDF(p - eps.xyy).x,
		GetSDF(p + eps.yxy).x - GetSDF(p - eps.yxy).x,
		GetSDF(p + eps.yyx).x - GetSDF(p - eps.yyx).x);
	return normalize(nor);
}

bool march(vec3 ro, vec3 rd, float far, int iters, float prec, out vec2 md)
{
	float d = 0.;
	vec3 p = ro;
	vec2 s = GetSDF(p);
	for (int i=0;i<iters;i++)
	{
		if (abs(s.x)<d*d*1e-6&&i>0) break;
		if (d>far) return false;
		s = GetSDF(p);
		d += s.x * prec;
		p = ro+rd*d;
	}
	md = vec2(d, s.y);
	return true;
}

void mainImage(inout vec4 fragColor, in vec2 fragCoord)
{
	vec3 color = vec3(0.0);
	
	vec2 md = vec2(0);
	vec3 ro = getRayOrigin();
	vec3 rd = getRayDirection();
	vec3 eye = ro;
	
	float maxDistance = rmFar;
	vec3 nor = vec3(0);
	vec2 d = vec2(0);
	if (getCullingPoint(eye, rd, ro, d, maxDistance, nor))
	{
		float s = GetSDF(ro).x;
		if (s < 0.0)
		{
			CalcDepth(rd, d.x); 
			
			// section color
			// https://www.shadertoy.com/view/3l23RK
			color = vec3(1.0) - sign(s) * vec3(0.1,0.4,0.7);
			color *= 1.0 - exp(-2.0*abs(s));
			color = mix( color, vec3(1.0), 1.0-smoothstep(0.0,0.02,abs(s)) );
			
			// surface light
			vec3 ld = vec3(0,1,0);
			float diff = dot(nor, ld) * .5 + .5;
			float spe = pow(max(dot(-rd, reflect(-ld, nor)), 0.0), 32.0);
			color += diff * 0.25 + spe * 0.5;
		}
		else
		{
			if (march(ro, rd, maxDistance, rmIters, rmPrec, md))
			{
				CalcDepth(rd, d.x + md.x); 
				
				vec3 p = ro + rd * md.x;
				nor = getNor(p, normalPrec);
				float my = GetSDF(p).y;
				vec3 matColor = vec3(0);
				if (my < 1.) matColor = matColor0;
				else if (my < 2.) matColor = matColor1;
				
				// sdf light
				vec3 ld = vec3(0,1,0);
				float diff = dot(nor, ld) * .5 + .5;
				float spe = pow(max(dot(-rd, reflect(-ld, nor)), 0.0), 32.0);
				color += matColor * 0.5 + diff * 0.25 + light0Col * spe * 0.5;
			}
			else
			{
				discard;
			}
		}
	}
	else
	{
		discard;
	}
	
	fragColor = vec4(sqrt(color*color*1.2), 1.0);
}

void main(void)
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	mainImage(color, gl_FragCoord.xy);
	color.a = 1.0;
	fragColor = color;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Demo_Buffer_Attachments() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@FRAMEBUFFER 

FORMAT(float)
COUNT(2) // two attachments
WRAP(clamp)
FILTER(linear)
MIPMAP(true)
SIZE(512)
//RATIO(1.5 or picture:file.jpeg)

@UNIFORMS

uniform float(time) uTime;
uniform int(frame) uFrame;
uniform float(deltatime) uDeltaTime;
uniform vec2(buffer:target=0) uSize;

uniform(mouse) vec4(mouse:2pos_2click)	mouse;

uniform sampler2D(buffer:target=0) uPatternBuffer; // attachment 0 (simulation)
uniform sampler2D(buffer:target=1) uColorBuffer; // attachment 1 (display)

uniform(pattern) int(combobox:custom,base,solitons,movitons,worms) uPattern; 

uniform(grayscott:0) float(0:0.2:0.05) uFeed;
uniform(grayscott:1) float(0:0.2:0.15) uKill;

uniform(grayscott:3) vec2(0:1:0.2,0.1) uDiff;

@NOTE

you can switch the buffer preview by select 0 or 1 in menubar or type key 0 or 1
select mouse and draw shape if you want
you can also add more iterations for spped up the simulation

@VERTEX

layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragPattern; // attachment 0 (simulation)
layout(location = 1) out vec4 fragColor; // attachment 1 (display)

void main(void)
{
	vec2 s = uSize;
	vec2 g = gl_FragCoord.xy;
	
	if (uFrame < 1)
	{
		float gy = g.y /s.y*2.-1.;
		gy = mod(gy, 0.5) - 0.25;
		gy += (sin(g.x/s.x * 100.) * 0.5+0.5) * 2./s.y;
		if (abs(gy) < 2./s.y)
		{
			fragPattern = vec4(0,1,0,1);
		}
		else
		{
			fragPattern = vec4(1,0,0,1);
		}
	}
	else
	{
		vec4 c = texture(uPatternBuffer, fract(g/s));
		
		vec4 l = texture(uPatternBuffer, fract((g+vec2(-1,0))/s));
		vec4 r = texture(uPatternBuffer, fract((g+vec2(1,0))/s));
		vec4 b = texture(uPatternBuffer, fract((g+vec2(0,-1))/s));
		vec4 t = texture(uPatternBuffer, fract((g+vec2(0,1))/s));
		
		vec4 lp = l+r+b+t - 4.0 * c;
		
		float feedRate = 0.0;
		float killRate = 0.0;
		
		if (uPattern == 0) // base
		{
			feedRate = uFeed;
			killRate = uKill;
		}
		else if (uPattern == 1) // base
		{
			// base 0.037, 0.06
			feedRate = 0.037;
			killRate = 0.06;
		}
		else if (uPattern == 2) // solitons
		{
			// solitons 0.03,0.062
			feedRate = 0.03;
			killRate = 0.062;
		}
		else if (uPattern == 3) // movitons
		{
			// movitons 0.014, 0.054
			feedRate = 0.014;
			killRate = 0.054;
			
		}
		else if (uPattern == 4) // worms
		{
			// worms 0.078,0.061
			feedRate = 0.078;
			killRate = 0.061;
		}
		
		float reac = c.r * c.g * c.g;
		float du = uDiff.x * lp.r - reac + feedRate * (1.0 - c.r);
        float dv = uDiff.y * lp.g + reac - (feedRate + killRate) * c.g;
        vec2 dst = c.rg + vec2(du, dv);
		
		fragPattern = vec4(clamp(dst,0.,1.),0,1);
		
		if (mouse.z > 0.)
		{
			if (length(g - mouse.xy) < 1.)
			{
				fragPattern = vec4(0,1,0,1);
			}
		}
	}
	
	// display => quick color via sinus
	fragColor = sin(10. * fragPattern) * 0.5 + 0.5;
	fragColor.a = 1.0;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Demo_Compute() {
    ShaderInfos comp;
    comp.header +=
        u8R"(
@UNIFORMS

uniform(hidden) int(frame) uFrame;

uniform sampler2D(compute:format=R32F:filter=linear:mipmap=true:binding=0) uPheromone; 
uniform sampler2D(compute:format=RGBA32F:filter=linear:mipmap=true:binding=1) uParticle; 
uniform vec2(compute) uSize;

uniform vec4(mouse:2pos_2click) uMouse;
uniform float(0:500:100) uMouseRadius;
uniform float(0:50:20) uSensorDistance;
uniform float(0:3.14159:0.15) uSensorAngle;
uniform int(0:10:1) uSensorRadius;
uniform float(0:512:240) uZoneRadius;
uniform float(0:1.0:0.95) uDissipation;
uniform float(0:10:1:1) uParticleSpeed;
uniform float(0:1:0.1:0.01) uAccumStep;
uniform sampler2D(picture) uPicture;

@COMPUTE SIZE(2048,2048,1) WORKGROUPS(64,64,1)

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (R32F, binding = 0) uniform image2D imagePheromone;
layout (RGBA32f, binding = 1) uniform image2D imageParticle;

vec2 Random(vec2 p)
{
	vec3 a = fract(p.xyx * vec3(123.34, 234.35, 345.65));
	a += dot(a, a + 34.45);
	return fract(vec2(a.x * a.y, a.y * a.z));
}

vec2 loop(vec2 p)
{
	return mod(p, uSize);
}

ivec2 loop_i(vec2 p)
{
	return ivec2(mod(p, uSize));
}

float GetPheromoneSample(ivec2 FP)
{
	return texture(uPheromone, (loop(FP) + 0.5) / uSize).x;
}

vec4 GetParticle(ivec2 FP)
{
	return texture(uParticle, (loop(FP) + 0.5) / uSize);
}

float GetSensorValue(ivec2 FP)
{
	float F = 0.0;
	
	if (uSensorRadius == 0)
	{
		F = GetPheromoneSample(FP);
	}
	else
	{
		const float weight = 1.0 / pow(uSensorRadius * 2.0 + 1.0, 2.0);
		for (int x = -uSensorRadius; x <= uSensorRadius; x++)
			for (int y = -uSensorRadius; y <= uSensorRadius; y++)
				F += GetPheromoneSample(FP + ivec2(x, y)).x * weight;
	}
	
	return F;
}

ivec2 GetForward(in vec4 FParticle, in float FSensorAngle)
{
	FParticle.rg += uSensorDistance * vec2(cos(FParticle.b + FSensorAngle), sin(FParticle.b + FSensorAngle));
	return loop_i(FParticle.rg);
}

bool IfInZone(vec2 p)
{
	vec2 dp = p - uSize * 0.5f;
	float sdf = length(dp) - uZoneRadius;
	//float sdf = length(max(abs(dp) - vec2(uZoneRadius, uZoneRadius), 0.0f)) - 1.0f;
	return (sdf < 0.0f);
	return true;
}

void main() 
{
	ivec2 id = ivec2(gl_GlobalInvocationID.xy);

	float PHE = GetPheromoneSample(id);
	
	// Pheromones Diffusion
	float FV = -GetPheromoneSample(id);
	FV += GetPheromoneSample(id + ivec2(-1, 0)) * 0.25f; // l
	FV += GetPheromoneSample(id + ivec2(0, 1)) * 0.25f; // t
	FV += GetPheromoneSample(id + ivec2(1, 0)) * 0.25f; // r
	FV += GetPheromoneSample(id + ivec2(0, -1)) * 0.25f; // b
	PHE += FV;

	// Pheromones Dissipation
	PHE *= uDissipation;

	// Write new Pheromone
	imageStore(imagePheromone, id, vec4(PHE,0,0,1));

	// Current Particle
	vec4 particle = GetParticle(id);

	// Particle Creation
	if (IfInZone(id))
	{
		if (uMouse.z > 0.0 && length(uMouse.xy - id.xy) < uMouseRadius)
		{
			//if (particle.a < 0.5)
			{
				particle.rg = id;
				particle.b = Random(id.yx + uFrame).x * 6.28318;
				particle.a = 1.0;
				particle.rg = loop(particle.rg);
				imageStore(imageParticle, id, particle);
				imageStore(imagePheromone, ivec2(particle.rg), vec4(1.0, 0.0, 0.0, 1));
			}
		}
	}

	if (particle.a > 0.5)
	{
		// Particle Moves
		ivec2 FP = GetForward(particle, 0.0);
		ivec2 FLP = GetForward(particle, -uSensorAngle); // left
		ivec2 FRP = GetForward(particle, uSensorAngle); // right

		// Particle Sensors Front, Left, Right
		float F = GetSensorValue(FP);
		float FL = GetSensorValue(FLP);
		float FR = GetSensorValue(FRP);

		// Particle Motor Stage
		if (F < 1.0) // no pheromone
		{
			// Move Forward
			vec2 newPos = particle.rg + uParticleSpeed * vec2(cos(particle.b), sin(particle.b));
			newPos = loop(newPos);
			if (IfInZone(newPos))
			{
				// deposit trail in new location
				float FV = GetPheromoneSample(ivec2(particle.rg));
				imageStore(imagePheromone, ivec2(particle.rg), vec4(FV + uAccumStep, 0, 0, 0));
				particle.rg = newPos;
			}
			else
			{
				// choose random new orientation
				particle.b = Random(particle.gr + uFrame).x * 6.28318;
			}
		}
		else
		{
			// choose random new orientation
			particle.b = Random(particle.rg).x * 6.28318;
		}

		// Particle Sensor Stage
		if ((F > FL) && (F < FR))
		{
			@CONFIG_START NORMAL
			@CONFIG_END
			
			@CONFIG_START MULT
			particle.b *= 1.5;
			@CONFIG_END
			
			@CONFIG_START PICTURE
			vec2 uv = fract(vec2(id) / uSize);
			vec4 pix = texture(uPicture, uv);
			particle.b *= ((pix.x + pix.y + pix.z + pix.w) / 4.0);// * 2.0 - 1.0;
			@CONFIG_END
			
			@CONFIG_START ADD
			particle.b = cos(particle.a) * sin(particle.a);
			@CONFIG_END
		}
		else if ((F < FL) && (F < FR))
		{
			// rotate randomly left or right by sensor angle
			float rand = Random(particle.gr + uFrame).x * 2.0 - 1.0;
			particle.b += rand * uSensorAngle;
		}
		else if (FL < FR)
		{
			// rotate right
			particle.b += uSensorAngle;
		}
		else if (FR < FL)
		{
			// rotate left
			particle.b -= uSensorAngle;
		}
	}

	/*
	if (IfInZone(particle.rg))
	{
		particle.a = 0.0;
	}
	imageStore(imageParticle, id, particle);
	*/
	
	if (IfInZone(particle.rg))
	{
		imageStore(imageParticle, id, particle);
	}
	
	if (uFrame < 2)
	{
		imageStore(imageParticle, id, vec4(0));
		imageStore(imagePheromone, id, vec4(0));
	}
}

)";
    ShaderInfos main;
    main.header +=
        u8R"(
@FRAMEBUFFER 

SIZE(1024)

@UNIFORMS

uniform vec2(buffer) uSize;

uniform sampler2D(compute:file=compute_physarum:target=uPheromone) uPheromone;

@VERTEX QUAD()

layout(location = 0) in vec2 a_pos;

void main()
{
	gl_Position = vec4(a_pos, 0.0, 1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;

vec4 getPheromone(vec2 p)
{
	return texture(uPheromone, p / uSize, 0.0);
}

vec4 getPheromoneInv(vec2 p)
{
	return getPheromone(p);
}

const vec3 lightDiffuse = vec3(0.191553,0.267195,0.373984);
const vec3 lightSpecular = vec3(0.243903,1,0);
const vec3 lightDirection = vec3(0.08232,-0.24085,-0.58841);
const float specularPower = 20.0;

void main(void)
{
	float e = 10.0 / min(uSize.x, uSize.y);
	float f = getPheromoneInv(gl_FragCoord.xy).x;
	float fx = (f-getPheromoneInv(gl_FragCoord.xy + vec2(1,0)).x)/e;
	float fy = (f-getPheromoneInv(gl_FragCoord.xy + vec2(0,1)).x)/e;
	vec3 n = normalize(vec3(0,0,1) - vec3(fx,fy,0.0));
	
	float diff = max(dot(vec3(0,0,1), n), 0.0);
	float spec = pow(max(dot(normalize(lightDirection), reflect(vec3(0,0,1),n)), 0.0), specularPower);
		
    fragColor.rgb = lightDiffuse * diff + lightSpecular * spec; 
	fragColor.rgb *= 1.5;
	
	vec3 pix = getPheromoneInv(gl_FragCoord.xy).rgb;
	fragColor.rgb = pix.rrr;
	//fragColor.rgb = n * 0.5 + 0.5;
	fragColor.a = 1.0;
}
)";
    main.childs.push_back(comp);
    return main;
}

ShaderInfos CodeGenerator::Get_Shader_Demo_Instanced() {
    ShaderInfos infos;
    infos.header +=
        u8R"(
@UNIFORMS

uniform mat4(camera:mvp) cam;
uniform float(maxinstances) countInstances;
uniform float(0:1:0.3:0.05) space; // space between cubes

@VERTEX POINTS(0:36:36) INSTANCES(0:1000:64) DISPLAY(TRIANGLES)

layout(location = 0) in float vertexId;
out vec4 v_color;

flat out float instanceID;

vec3 quad(float vId)
{
	float vtxId = mod(vId, 6.);
	float quadId = floor(vId / 6.);
	
	vec2 p = vec2(0);
	
	if (vtxId < 1.) p = vec2(0,0);
	else if (vtxId < 2.) p = vec2(1,0);
	else if (vtxId < 3.) p = vec2(1,1);
	else if (vtxId < 4.) p = vec2(1,1);
	else if (vtxId < 5.) p = vec2(0,1);
	else if (vtxId < 6.) p = vec2(0,0);
	
	return vec3(p * 2. - 1., quadId);
}

vec4 cube(float vId)
{
	vec3 quadPos = quad(vId);
	
	float quadId = mod(quadPos.z, 6.);
	float cubeId = floor(quadPos.z / 6.);
	
	vec3 p = vec3(0);
	
	if (quadId < 1.) 		p = vec3(quadPos.x, -1, quadPos.y);
	else if (quadId < 2.) 	p = vec3(-1, quadPos.y, quadPos.x);
	else if (quadId < 3.) 	p = vec3(quadPos.y, quadPos.x, -1);
	else if (quadId < 4.) 	p = vec3(quadPos.x, 1, quadPos.y);
	else if (quadId < 5.) 	p = vec3(1, quadPos.y, quadPos.x);
	else if (quadId < 6.) 	p = vec3(quadPos.y, quadPos.x, 1);
	
	return vec4(p, cubeId);
}

vec3 to3D( float idx, vec3 side ) 
{
    float z = floor(idx / (side.x * side.y));
    idx -= ceil(z * side.x * side.y);
    float y = floor(idx / side.x);
    float x = mod(idx, side.y);
    return vec3(x,y,z);
}

void main()
{
	instanceID = gl_InstanceID / (countInstances - 1.0);
	
	vec4 ci = cube(vertexId);
	
	float side = floor(pow(countInstances, 1.0 / 3.0));
	vec3 instanceCubeCenter = to3D(gl_InstanceID, vec3(side)) - side * 0.5;
	instanceCubeCenter *= 1.0 + space;
	instanceCubeCenter += space * 0.5;
	v_color = vec4(ci.xyz * 0.5 + 0.5, 1.0);
	gl_Position = cam * vec4(v_color.xyz + instanceCubeCenter, 1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;

in vec4 v_color;

void main(void)
{
	fragColor = v_color;
}
)";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Demo_Scene() {
    CTOOL_DEBUG_BREAK;

    ShaderInfos infos;
    infos.header += u8R"()";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Demo_Multipass_One_File_2_buffers() {
    CTOOL_DEBUG_BREAK;

    ShaderInfos infos;
    infos.header += u8R"()";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Demo_Multipass_One_File_3_buffers() {
    CTOOL_DEBUG_BREAK;

    ShaderInfos infos;
    infos.header += u8R"()";
    return infos;
}

ShaderInfos CodeGenerator::Get_Shader_Demo_Multipass_One_File_4_buffers() {
    CTOOL_DEBUG_BREAK;

    ShaderInfos infos;
    infos.header += u8R"()";
    return infos;
}
