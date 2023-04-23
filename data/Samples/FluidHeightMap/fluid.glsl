@FRAMEBUFFER SIZE(1000,1000) FILTER(linear) COUNT(3) /*RATIO(picture:laby2.jpg)*/ /*RATIO(1.25)*/ /*SIZE(picture:laby2.jpg)*/

@UNIFORMS

uniform(hidden) int(frame) uFrame;
uniform(hidden) float(deltatime) uDeltaTime;

////////////////////////////////////////////

uniform(buffer) sampler2D(buffer:target=0) navierScottBuffer; // buffer 0
uniform(buffer) sampler2D(buffer:target=1) grayScottBuffer; // buffer 1
uniform(buffer) sampler2D(buffer:target=2) colorBuffer; // buffer 2
uniform(hidden) vec2(buffer:target=0) size;

////////////////////////////////////////////

uniform(color) vec3(0:10:7.7,6.0,4.1) color;

uniform(physic) float(0.0:1.0:0.1) force;

uniform(prism) float(0.00001:0.1:0.01) constraint;
uniform(prism) float(-1.0:1.0:0.0) effect;

uniform(emit) float(1.0:20.0:5.0) emitSize;
uniform(emit) int(1:10:1) count;
uniform(emit) float(0.0:0.5:0.3) radius;
uniform(emit) float(checkbox:true) emit;
uniform(emit) float(checkbox:true) ink;

uniform(shape) int(combobox:circle,triangle,quad:quad) shapeType;

@VERTEX // default is quad()

layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor; // buffer 0
layout(location = 1) out vec4 fragColor1; // buffer 1
layout(location = 2) out vec4 fragColor2; // buffer 2

#define texNS(g) texture(navierScottBuffer, (g)/size)
#define texGS(g) texture(grayScottBuffer, (g)/size)

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
	
	vec4 a = texNS(g+vec2(1,0));
	vec4 b = texNS(g+vec2(0,1));
	vec4 c = texNS(g+vec2(-1,0));
	vec4 d = texNS(g+vec2(0,-1));
	
	fragColor = texNS(g-texNS(g).xy); // last state
	
	vec2 gp = vec2(a.z-c.z,b.z-d.z); // pressure gradiant
	
	float disp = 0.05;
	
	fragColor.xyz = vec3(
		fragColor.x + gp.x,
		fragColor.y + gp.y,
		1./4. * (a.z + b.z + c.z + d.z) - disp * (c.x - a.x + d.y - b.y));
	
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
			if (length(g-(p)) < emitSize)
			{
				fragColor.xy = fragColor.xy * (1.0 - force) + force * (dir);
				if (ink > 0.5)
				{
					fragColor.w = 1.0;
				}
			}				
		}
	}
	
	// wall
	/*vec2 p = g - size * 0.5;
	vec2 dim = vec2(200);
	if (shape(g-size*0.5, dim) < 0.01) 
	{
		fragColor.w = fragColor.z / (shape(fragColor.xy, vec2(constraint)));
		fragColor.xy *= effect;
		fragColor.z *= viscosity; // viscosité
	}*/
	//wallCircle(size*0.5, 200.);
	
	fragColor2 = 0.5+0.5*sin((texNS(g).z*5. + texNS(g).w) * vec4(color,1));
	fragColor2 = clamp(fragColor2, 0.,1.);
	
	// gray scott
	vec2 diffusionCoef = vec2(.3);
    float feedCoef = 0.06;
    float killCoef = 0.04;
            
	vec4 aa = texGS(g+vec2(1,0));
	vec4 bb = texGS(g+vec2(0,1));
	vec4 cc = texGS(g+vec2(-1,0));
	vec4 dd = texGS(g+vec2(0,-1));
	vec4 lap = 0.25 * (aa + bb + cc + dd);
	
    vec2 ab = texGS(g).xy;
    vec2 lp = lap.xy;
       
    float reaction = ab.x * ab.y * ab.y;
    vec2 diffusion = diffusionCoef * lp;
    float feed = feedCoef * (1. - ab.x);
    float kill = (feedCoef + killCoef) * ab.y;
    ab += diffusion + vec2(feed - reaction, reaction - kill);
	fragColor1 = vec4(ab, 0, 1);
}
