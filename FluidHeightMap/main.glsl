@FRAMEBUFFER /*SIZE(800,600)*/ //RATIO(picture:file.jpeg)

@UNIFORMS

uniform 		float		(time) time; // time
uniform 		float		(0.0:100.0:1.0) freq; // frequence
uniform 		mat4		(camera:mvp) cam;
uniform 		float		(maxpoints) maxpoints;
uniform 		float		(0.0:100.0:5.0) altitude;

uniform(hidden) sampler2D	(buffer:target=0) uBuffer;
uniform(hidden) sampler2D	(buffer:target=2:file=fluid) uFluid;		// clid buffer
uniform(hidden) vec2		(buffer:target=2:file=fluid) uFluidSize;	// clid buffer size

uniform 		float		(checkbox) uUsePicture;
uniform(hidden) sampler2D	(picture:file=monalisa.jpg) uPicture;
uniform(hidden) vec2		(picture:file=monalisa.jpg) uPictureSize;

uniform 		float		(usegeometry) uUseGeometry;

uniform 		float		(0.0:100.0:0.0) uDfPos;
uniform 		float		(0.0:200.0:0.0) uRadius;

uniform 		int			(1:16:16) uMaxIter;
uniform 		int			(1:10:5) uMengerIter;

@VERTEX POINTS(100:10000000:900000) // point slider

layout(location = 0) in float vertexId;
out vec4 v_color;

mat3 RotX(float a){a = radians(a); return mat3(1.,0.,0.,0.,cos(a),-sin(a),0.,sin(a),cos(a));}
mat3 RotY(float a){a = radians(a); return mat3(cos(a),0.,sin(a),0.,1.,0.,-sin(a),0.,cos(a));}
mat3 RotZ(float a){a = radians(a); return mat3(cos(a),-sin(a),0.,sin(a),cos(a),0.,0.,0.,1.);}
mat3 Rot(vec3 a){return RotX(a.x) * RotY(a.y) * RotZ(a.z);}

mat3 lookat(vec3 d, vec3 up)
{
	vec3 w = normalize(d),u = normalize(cross(w,up));
    return (mat3(u,cross(u,w),w));
}

float getDf(vec3 p)
{
	float plane = p.y;
	float sp = length((p+vec3(0,uDfPos,0)).yz) - uRadius;
	return min(plane, sp) ;
}

vec3 getNor(vec3 p)
{
	vec2 e = vec2( 0.0001, 0. );
    vec3 n = vec3(
		getDf(p+e.xyy) - getDf(p-e.xyy),
		getDf(p+e.yxy) - getDf(p-e.yxy),
		getDf(p+e.yyx) - getDf(p-e.yyx) );
    return normalize(n);
}

struct pos_nor
{
	vec3 p;
	vec3 n;
};

pos_nor getPos(vec2 v, float altitude)
{
	pos_nor pn;
	vec3 ro = vec3(v.x, altitude, v.y);
	vec3 rd = vec3(0,-1,0);
	float d = 0., s = 1.;
	vec3 p = ro + rd * d;
	for (int i=0;i<10;i++)
	{
		if (s < 0.001) break;
		s = getDf(p);
		d += s;
		p = ro + rd * d; 
	}
	if (getDf(vec3(v.x, 0, v.y)) < 0.)
	{
		pn.p = p;
		pn.n = getNor(pn.p);
	}
	else
	{
		pn.p = vec3(v.x, 0., v.y);
		pn.n = vec3(0,1,0);
	}
	return pn;
}

vec4 getQuad(in float vId)
{
	vec3 p = vec3(0);
	
	float index = mod(vId, 6.);
	
	if (index < 1.) p.xz = vec2(-1,-1);
	else if (index < 2.) p.xz = vec2(1,-1); 
	else if (index < 3.) p.xz = vec2(1,1);
	else if (index < 4.) p.xz = vec2(-1,-1);
	else if (index < 5.) p.xz = vec2(1,1);
	else if (index < 6.) p.xz = vec2(-1,1);
	
	index = floor(vId / 6.);
	
	return vec4(p, index);
}

struct cubeStruct
{
	vec3 p;
	vec3 n;
	float i;
};

cubeStruct getCube(in float vId)
{
	vec4 pi = getQuad(vId);
	vec3 p = pi.xyz;
	float idQuad = floor(vId / 6.);
	
	vec3 n = vec3(0);
	
	float index = mod(idQuad, 6.);
	
	if (index < 1.) p = vec3(p.x, p.y + 1., p.z), n = vec3(0,1,0);
	else if (index < 2.) p = vec3(p.x, p.z, p.y + 1.), n = vec3(0,0,1);
	else if (index < 3.) p = vec3(p.x, p.z, p.y - 1.), n = vec3(0,0,-1);
	else if (index < 4.) p = vec3(p.y + 1., p.x, p.z), n = vec3(1,1,0);
	else if (index < 5.) p = vec3(p.y - 1., p.x, p.z), n = vec3(-1,0,0);
	else if (index < 6.) p = vec3(p.x, p.y - 1., p.z), n = vec3(0,-1,0);
	
	index = floor(idQuad / 6.);
	
	cubeStruct res;
	res.p = p;
	res.n = n;
	res.i = index;
	
	return res;
}

vec4 getTexture(vec2 uv)
{
	vec4 res = vec4(0);
	
	if (uUsePicture > 0.5)
		res = texture(uPicture, uv);
	else
		res = texture(uFluid, uv);
	
	res = clamp(res, 0., 1.);
	
	return res;
}

void main()
{
	vec3 p = vec3(0);
	
	#include "variante.glsl"//:cube // put ':section' is for force the use of this section name
	
	gl_Position = cam * vec4(p, 1);
	
	if (uUseGeometry > 0.5)
		gl_Position = vec4(p, 1);
	
	v_color = vec4(chan.rgb, 1);
}

@GEOMETRY 

// selectabe config in code via comobobox on left pane
@CONFIG_START TEST_BASE

layout(location = 0) in float v_vertexId[]; // for have position in others shaders
out vec4 v_color ; // for have vertex color in others shaders

layout(triangles) in; // input primitive(s) (must match the current render mode) (points / lines / triangles / lines_adjacency / triangles_adjacency)
layout(triangle_strip, max_vertices=3) out; // output primitive(s) (points / line_strip / triangle_strip)

/*
in gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
} gl_in[];
*/

void main()
{
	vec3 p0 = gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz;
	
	vec3 faceCenter = (p0 + p1 + p2) / 3.;

	vec3 faceNormal = normalize(cross(p0-p2,p0-p1));
	
	gl_Position = cam * vec4(p0, 1.0);
	v_color = vec4(p0.y);
	EmitVertex();
	
	gl_Position = cam * vec4(p1, 1.0);
	v_color = vec4(p1.y);
	EmitVertex();
	
	gl_Position = cam * vec4(p2, 1.0);
	v_color = vec4(p2.y);
	EmitVertex();
	
	EndPrimitive();
}

@CONFIG_END

@CONFIG_START TEST_CONE

layout(location = 0) in float v_vertexId[]; // for have position in others shaders
out vec4 v_color ; // for have vertex color in others shaders

layout(triangles) in; // input primitive(s) (must match the current render mode) (points / lines / triangles / lines_adjacency / triangles_adjacency)
layout(triangle_strip, max_vertices=4) out; // output primitive(s) (points / line_strip / triangle_strip)

/*
in gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
} gl_in[];
*/

void main()
{
	vec3 p0 = gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz;
	
	vec3 faceCenter = (p0 + p1 + p2) / 3.;

	vec3 faceNormal = normalize(
				cross(	p0 - p2,
						p0 - p1));
	
	gl_Position = cam * vec4(p0, 1.0);
	v_color = vec4(p0.y);
	EmitVertex();
	
	gl_Position = cam * vec4(p1, 1.0);
	v_color = vec4(p1.y);
	EmitVertex();
	
	gl_Position = cam * vec4(p2, 1.0);
	v_color = vec4(p2.y);
	EmitVertex();
	
	gl_Position = cam * vec4(faceCenter + faceNormal * 2., 1.0);
	v_color = vec4(faceCenter.y);
	EmitVertex();
	
	EndPrimitive();
}

@CONFIG_END

@FRAGMENT

layout(location = 0) out vec4 fragColor;
in vec4 v_color;

void main(void)
{
	fragColor = v_color;
}
