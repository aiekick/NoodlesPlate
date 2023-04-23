@FRAMEBUFFER COUNT(2)/*FORMAT(float or byte)*/ /*COUNT(1 to 8)*/ /*WRAP(clamp or repeat or mirror)*/ /*FILTER(linear or nearest)*/ /*MIPMAP(false or true)*/ /*SIZE(800,600 or picture:file.jpeg)*/ //RATIO(1.5 or picture:file.jpeg)


@UNIFORMS

uniform float(time) iTime;

uniform() mat4(camera:mvp) _cam;
uniform() vec2(buffer:0) iResolution;

uniform(tesselation) float(-2.0:2.0:0.5) tessDisplace;
uniform(tesselation) float(0.0:100.0:100.0) tessDisplaceFactor;
uniform(tesselation) float(0:100:50:1) tessLevelInner;
uniform(tesselation) float(1:100:100:1) tessLevelOuter;
uniform(tesselation) float(checkbox:false) showWireFrameInMSAAx2;

uniform(normals:0) float(checkbox) calcNormals;
uniform(normals:1:calcNormals==true) float(0.0:1.0:0.0) RatioNorCol;
uniform(normals:2) float(checkbox) useNormalAsColor;

uniform(vertex:0) float(checkbox) useVertexColor;
uniform(vertex:1:useVertexColor==true) float(0.0:1.0:0.0) RatioVertCol;

uniform sampler2D(picture:file=tex09.jpg) iChannel0;

uniform(shape) float(0:10:2) radius;

uniform(light) float(0:1:0.6) diffRatio;
uniform(light) vec3(color:1.0,1.0,1.0) diffColor;
uniform(light) float(0:1:0.6) speRatio;
uniform(light) float(0:1:0.6) colRatio;

@COMMON

vec4 pics(vec2 p)
{
	#define ff length(fract(p*=mat2(7, -5, 5, 7)*.1) - .5)
	p-=.5+iTime*0.1;
    p*=15.;
    return vec4(min(min(ff, ff), ff)/.7);
}

vec4 displacement(vec3 p)
{
	float sc = 0.2;
	return mix(mix(pics(p.xz * sc), pics(p.xy * sc), 0.5), pics(p.yz * sc), 0.5);
}

@VERTEX MESH(ico.obj)

#extension GL_ARB_explicit_attrib_location : enable
layout(location = 0) in vec3 a_position; // Current vertex position
layout(location = 1) in vec3 a_normal; // Current normal at this vertex
layout(location = 2) in vec4 a_color; // Current color at this vertex
out vec3 v_position; // for have position in others shaders
out vec3 v_normal; // for have normal in others shaders
out vec4 v_color; // for have vertex color in others shaders

out vec3 g_position;

void main()
{
	gl_PointSize = 3.;
	v_position = a_position;
	v_normal = a_normal;
	v_color = a_color;
	
	g_position = normalize(a_position) * radius;
	gl_Position = _cam * vec4(g_position,1);
}

@TESSCONTROL

// define the number of CPs in the output patch
layout (vertices = 3) out;

// attributes of the input CPs
in vec3 v_position[]; // for have position in others shaders
in vec3 v_normal[]; // for have normal in others shaders

out vec3 v_position_tess_control[];
out vec3 v_normal_tess_control[];

void main()
{
    // Set the control points of the output patch
    v_position_tess_control[gl_InvocationID] = v_position[gl_InvocationID];
	v_normal_tess_control[gl_InvocationID] = normalize(v_position[gl_InvocationID]);
	
	if (gl_InvocationID == 0)
	{
		// Define / Calculate the tessellation levels
		gl_TessLevelOuter[0] = tessLevelOuter;
		gl_TessLevelOuter[1] = tessLevelOuter;
		gl_TessLevelOuter[2] = tessLevelOuter;
		gl_TessLevelOuter[3] = tessLevelOuter;
		
		gl_TessLevelInner[0] = tessLevelInner;
		gl_TessLevelInner[1] = tessLevelInner;
	}
} 

@TESSEVAL

layout(triangles, equal_spacing, ccw) in;

in vec3 v_position_tess_control[]; // for have position in others shaders
in vec3 v_normal_tess_control[]; // for have normal in others shaders

out vec3 v_position; // for have position in others shaders
out vec3 v_normal; // for have normal in others shaders
out vec3 g_position;

vec3 interpolateV3(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
} 

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
    v_position = interpolateV3(v_position_tess_control[0], v_position_tess_control[1], v_position_tess_control[2]);
	v_normal = interpolateV3(v_normal_tess_control[0], v_normal_tess_control[1], v_normal_tess_control[2]);

	// Displace the vertex along the normal
	vec3 n = normalize(v_position);
	v_position = n * radius;
	g_position = v_position + n * tessDisplace * displacement(v_position).x;
	gl_Position = _cam * vec4(g_position, 1.0);
	
	// new normal
	vec3 eps = vec3(0.01,0,0);
	float x = displacement(v_position + eps.xyz).x - displacement(v_position - eps.xyz).x;
	float y = displacement(v_position + eps.yxz).x - displacement(v_position - eps.yxz).x;
	float z = displacement(v_position + eps.zyx).x - displacement(v_position - eps.zyx).x;
	v_normal = normalize(n-vec3(x,y,z)*tessDisplace*tessDisplaceFactor);
} 

@FRAGMENT

in vec3 v_position; // for have position in others shaders
in vec3 v_normal; // for have normal in others shaders
in vec4 v_color; // for have vertex color in others shaders

#extension GL_ARB_explicit_attrib_location : enable
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalDebug;

in vec3 g_normal;
in vec3 g_position;

#include "space3d.glsl"

void main()
{
	vec4 col = vec4(0,0,0,1);

	vec3 n = v_normal.xyz;
	if (calcNormals > 0.5)
	{
		n = normalize(cross(dFdx(g_position), dFdy(g_position)));
	}
	else if (dot(g_normal,g_normal) > 0.)
	{
		n = g_normal;
	}
	
	normalDebug = vec4(n,1);
	
	vec2 uv = (gl_FragCoord.xy * 2.0 - iResolution) / min(iResolution.x, iResolution.y);
	
	//vec3 ro = getRayOrigin();
	vec3 rd = getRayDirection();
	
	float alpha = v_color.a * RatioVertCol;
	
	col.rgb = displacement(v_position).rgb;
	
	if (useNormalAsColor>0.5)
		col.rgb = (n * 0.5 + 0.5) * RatioNorCol;
	
	if (useVertexColor>0.5)
		col += v_color * RatioVertCol;
	
	if (showWireFrameInMSAAx2 > 0.5)
		col = (bitCount(gl_SampleMaskIn[0])<2) ? vec4(0.0) : col;
	
	vec3 ld = -vec3(0.8,0.5,0.2);
	float diff = pow(dot(n, -ld) * .5 + .5,2.0);
    float spe = pow(max(dot(-rd, reflect(ld, n)), 0.0), 32.0);
		    
	col.rgb = col.rgb * colRatio + diff * diffColor * diffRatio + spe * speRatio;
	
	fragColor = col;
	normalDebug = vec4(g_position,1);
}
