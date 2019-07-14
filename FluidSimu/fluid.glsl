@FRAMEBUFFER COUNT(2) /*SIZE(800,600)*/ //RATIO(picture:file.jpeg)

@UNIFORMS

uniform(hidden) int(frame) uFrame;
uniform(hidden) float(deltatime) uDeltaTime;

////////////////////////////////////////////

uniform(hidden) sampler2D(buffer:target=0) _buffer; // the name buffer is reserved so will cause weird error's if used as a name
uniform(hidden) sampler2D(buffer:target=1) buffer1;
uniform(hidden) vec2(buffer:target=0) size;

////////////////////////////////////////////

uniform(color) vec3(0:10:7.7,6.0,7.5) color;

uniform(physic) float(0.0:1.0:0.1) force;

uniform(prism) float(0.00001:0.1:0.01) constraint;
uniform(prism) float(-1.0:1.0:0.0) effect;
uniform(prism) float(0.99:1.0:1.0) viscosity;


uniform(emit) float(1.0:20.0:5.0) emitSize;
uniform(emit) int(0:50:1) count;
uniform(emit) float(0.0:0.5:0.3) radius;
uniform(emit) float(checkbox:true) emit;

uniform(shape) int(combobox:circle,triangle,quad:quad) shapeType;

@VERTEX

layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColor1;

#define tex(g) texture(_buffer, (g)/size)

// v => coord x,y / s => sens
#define emit(v,s) if (length(g-(v)) < emitSize) fragColor.xy = fragColor.xy * (1.0 - force) + force * (s), fragColor.w = 1.0

// v => coord x,y / d => diam
#define wallCircle(v,d) if (length(g-(v)) < (d)) fragColor.w = fragColor.z / (length(fragColor.xy)+0.0001), fragColor.xy *= 0.

float shape(vec2 p, vec2 d)
{
	float r = min(d.x, d.y);
	float disk = length(p) - r;
	float tri = max(abs(p.x)-p.y,p.y) - r;
	float quad = max(abs(p.x), abs(p.y)) - r;
	if (shapeType == 0) return disk;
	if (shapeType == 1) return tri;
		return quad;
}

void main(void)
{
	// x,y => pos
	// z => pressure
	// w => ink
	
	vec2 g = gl_FragCoord.xy;
	
	vec4 a = tex(g+vec2(1,0));
	vec4 b = tex(g+vec2(0,1));
	vec4 c = tex(g+vec2(-1,0));
	vec4 d = tex(g+vec2(0,-1));
	
	fragColor = tex(g-tex(g).xy); // last state
	
	vec2 gp = vec2(a.z-c.z,b.z-d.z); // pressure gradiant
	fragColor.xyz = vec3(
		fragColor.x + gp.x,
		fragColor.y + gp.y,
		0.25 * (a.z + b.z + c.z + d.z) - 0.05 * (c.x - a.x + d.y - b.y)) * viscosity;
	
	//fragColor.xy += vec2(cos(fragColor.x), sin(fragColor.y));
	
	if (uFrame < 1) fragColor = vec4(0);
	if (g.x < 1. || g.y < 1. || g.x > size.x - 1. || g.y > size.y - 1.) fragColor.xy *= 0.;
	
	if (emit > 0.5)
	{
		for (int i=0;i<count;i++)
		{
			float a = 6.28318 * float(i)/float(count);
			vec2 dir = vec2(cos(a),sin(a));
			vec2 p = size * 0.5 - dir * radius * min(size.x,size.y);
			emit(p, dir);
		}
	}
	
	// wall
	vec2 p = g - size * 0.5;
	vec2 dim = vec2(200);
	if (shape(g-size*0.5, dim) < 0.01) 
	{
		fragColor.w = fragColor.z / (shape(fragColor.xy, vec2(constraint)));
		fragColor.xy *= effect;
		fragColor.z *= viscosity; // viscosité
	}
	//wallCircle(size*0.5, 200.);
	
	fragColor1 = 0.5+0.5*sin(tex(g).w * vec4(color,1));
}
