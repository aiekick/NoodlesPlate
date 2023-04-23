@FRAMEBUFFER /*SIZE(800,600)*/ //RATIO(picture:file.jpeg)

@UNIFORMS

uniform float(time) time;

uniform(hidden) mat4(camera:mvp) cam;
uniform(hidden) float(maxinstances) countInstances;

uniform(volume:0) float(0.1:20:2) cellSize;
uniform(volume:1) float(0:10:2) maxVolume;

uniform(thick:0) float(checkbox) useThick;
uniform(thick:1:visible_if_checkbox[useThick]) float(0:1:0.1) thickness;

uniform(sdf:2) float(checkbox) useSDFNormalAsColor;
uniform(sdf:3) float(checkbox) useJuliaAsSDF;
uniform(sdf:0) float(0:100:15) sdfScale;
uniform(sdf:1) float(-2:2:0) sdfDilat;

uniform(loc) mat4(gizmo) sdfPos;

@VERTEX MESH(ico.obj) /*you can define no file for have choice in left pane */ INSTANCES(0:262144:25000) /* instace slider, you can alos just define value and you will not have a slider */

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_color;
out vec3 v_position;
out vec3 v_normal;
out vec4 v_color;

flat out float instanceID;

vec3 to3D( float idx, vec3 side ) 
{
    float z = floor(idx / (side.x * side.y));
    idx -= ceil(z * side.x * side.y);
    float y = floor(idx / side.x);
    float x = mod(idx, side.y);
    return vec3(x,y,z);
}

mat3 rx(float a){return mat3(1.,0.,0.,0.,cos(a),-sin(a),0.,sin(a),cos(a));}
mat3 ry(float a){return mat3(cos(a),0.,sin(a),0.,1.,0.,-sin(a),0.,cos(a));}
mat3 rz(float a){return mat3(cos(a),-sin(a),0.,sin(a),cos(a),0.,0.,0.,1.);}

float fullAtan(vec2 p)
{
    return step(0.0,-p.x)*3.14159 + sign(p.x) * atan(p.x, sign(p.x) * p.y);
}

float julia(vec2 p, vec2 v)
{
	vec2 z = p;
    vec2 c = v;
	float k = 1., h = 1.0;    
    for (int i=0;i<3;i++)
    {
        h *= 4.*k;
		k = dot(z,z);
        if(k > 4.) break;
		z = vec2(z.x * z.x - z.y * z.y, 2. * z.x * z.y) + c;
    }
	return sqrt(k/h)*log(k);
}

vec2 fractus(vec3 p, float o, float a)
{
	vec2 c = vec2(mix(0.2, -0.5, sin(a * 2.)), mix(0.5, 0.0, sin(a * 3.)));   
    float path = length(p.xz) - 3.;
    vec2 rev = vec2(path - o * 3., p.y - o);
    float aa = a + time;
    rev *= mat2(cos(aa),-sin(aa),sin(aa),cos(aa)); // rot near axis y    
    return vec2(julia(rev, c),1.);
}

float map(vec3 p)
{
	float t = time * 0.1;
	p/=sdfScale;
	if (useJuliaAsSDF>0.5)
	{
		float d = fractus(p, 0., fullAtan(p.xz)).x;;
		if (useThick > 0.5)
			return abs(d + thickness ) - thickness;
		return d;
	}
		
	p *= ry(t) * ry(t*1.5) * rz(-t*0.75);
	p += sin(p.zxy*3.36) * 0.44; // iq tech in  shader Sculpture III
	p += 0.5 * sin(p.zxy*3.36) * 0.44;
	if (useThick > 0.5)
		return abs(length(p) - 1. + thickness ) - thickness;
	return length(p) - 1.;
}

vec3 nor( vec3 pos, float prec )
{
	vec3 eps = vec3( prec, 0., 0. );
	vec3 nor = vec3(
	    map(pos+eps.xyy) - map(pos-eps.xyy),
	    map(pos+eps.yxy) - map(pos-eps.yxy),
	    map(pos+eps.yyx) - map(pos-eps.yyx) );
	return normalize(nor);
}

mat4 lookAt(vec3 eye, vec3 target, vec3 up) 
{
	vec3 vec = eye - target;
	if (dot(abs(vec), up) < 0.001)
	{
		//up = vec3(0,1,1);
	}
	vec3 zAxis = normalize(vec);
	vec3 xAxis = normalize(cross(up, zAxis));
	vec3 yAxis = cross(zAxis, xAxis);
	
	return mat4(xAxis, 0, yAxis, 0,	zAxis, 0, eye, 1);
}

void main()
{
	instanceID = gl_InstanceID / (countInstances-1.);
	
	float side = floor(pow(countInstances,1./3.));
	vec3 instanceCubeCenter = to3D(gl_InstanceID, vec3(side)) - side * 0.5;
	instanceCubeCenter *= cellSize;
	
	gl_PointSize = 3;
	v_position = a_position;
	v_normal = a_normal;
	v_color = vec4(a_normal*0.5+0.5,1);
	
	vec3 o = sdfPos[3].xyz * sdfScale * 0.5;
	vec3 p = instanceCubeCenter;
	float hit = map(p - o);
	if (hit < 0.0)
	{
		vec3 rd = normalize(-p);
		float d = length(p);
		p -= rd * d * sdfDilat;
		
		vec3 n = nor(p - o, 0.001);
			
		if (useSDFNormalAsColor>0.5)
		{
			v_color.rgb = v_color.rgb * 0.5 + n * 0.5;
		}
		
		vec3 pp = a_position;
		//pp.y *= 5.;
		//pp.xz *= 0.5;
		//pp = (vec4(pp,1) * lookAt(vec3(0), -n, vec3(1))).xyz; 
		//pp *= rx(-n.x) * ry(-n.y) * rz(-n.z);
		gl_Position = cam * vec4((pp + p) * maxVolume / sdfScale, 1);
	}
	else
	{
		gl_Position = cam * vec4(0);
		v_color = vec4(0);
	}
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;
in vec3 v_position;
in vec3 v_normal;
in vec4 v_color;

void main(void)
{
	vec3 n = normalize(cross(dFdx(v_position), dFdy(v_position)));
	fragColor = v_color;// * 0.5 + vec4(n,1);
}
