@FRAMEBUFFER

@UNIFORMS

uniform float(time) iTime;
uniform int(frame) iFrame;
uniform float(deltatime) iTimeDelta;
uniform vec3(buffer:target=0) iResolution;
uniform vec4(mouse:2pos_2click) iMouse;
uniform vec4(date) iDate;

@NOTE

//Possible flags to set for have specials fields.
//these fields are always displayed at the top of the Note pane
//uncomment if you want see the result
[[NAME]]:Protean Clouds
[[DATE]]:2019/05/27
[[URL]]:https://www.shadertoy.com/view/3l23Rh
[[USER]]:nimitz:https://www.shadertoy.com/user/nimitz

Protean clouds by nimitz (twitter: @stormoid)
https://www.shadertoy.com/view/3l23Rh
License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
Contact the author for other licensing options

	Technical details:

	The main volume noise is generated from a deformed periodic grid, which can produce
	a large range of noise-like patterns at very cheap evalutation cost. Allowing for multiple
	fetches of volume gradient computation for improved lighting.

	To further accelerate marching, since the volume is smooth, more than half the the density
	information isn't used to rendering or shading but only as an underlying volume	distance to 
	determine dynamic step size, by carefully selecting an equation	(polynomial for speed) to 
	step as a function of overall density (not necessarialy rendered) the visual results can be 
	the	same as a naive implementation with ~40% increase in rendering performance.

	Since the dynamic marching step size is even less uniform due to steps not being rendered at all
	the fog is evaluated as the difference of the fog integral at each rendered step.


i have tried to explain, what i understood of this code.
there is like all shaders many vairaible tuning for find the perfect result
some many vars have no other explia than creatte variation of color and shape.
i have explian de global goal of i understood.
if you have better explanation, of what some things rely, please share, we can modify comment

@VERTEX

layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@FRAGMENT

layout(location = 0) out vec4 fragColor;

@UNIFORMS

uniform(sdf) float(0:3:1.93) noiseStrength;
uniform(sdf) float(0:2:0.61) pointScale;
uniform(sdf) int(0:10:5) warpingIters;
uniform(sdf) float(0:2:0.57) dispScaleInLoop;
uniform(sdf) float(0:2:1.4) sinWarpScaleInLoop;

@FRAGMENT

// 2d rot
mat2 rot(in float a){float c = cos(a), s = sin(a);return mat2(c,s,-s,c);}
// used for generate noise in map loop
const mat3 m3 = mat3(0.33338, 0.56034, -0.71817, -0.87887, 0.32651, -0.15323, 0.15162, 0.69596, 0.61339)*noiseStrength;
// magnitude
float mag2(vec2 p){return dot(p,p);}
// linear step
float linstep(in float mn, in float mx, in float x){ return clamp((x - mn)/(mx - mn), 0., 1.); }
// is for mix over time colors and shape (blue and yellow based)
float prm1 = 0.;
// mouse
vec2 bsMo = vec2(0); 

// a path
vec2 disp(float t){ return vec2(sin(t*0.22)*1., cos(t*0.175)*1.)*2.; }

vec2 map(vec3 p)
{
    vec3 p2 = p;
    // tunnel path, affect 2d section center over tunnel axis z
    p2.xy -= disp(p.z).xy;
    // tunnel path, 2d rot over axis z
    p.xy *= rot(sin(p.z+iTime)*(0.1 + prm1*0.05) + iTime*0.09);
    // in fact with the last two lines, he generate two cylinders
    // the fist is the low freq tunnel for smooth point estimation
    // the second is the high freq tunnel, used for noise
    
    float cl = mag2(p2.xy); // tunnel low freq section distance
    float d = 0.; // noise displacement
    p *= pointScale; // point scale
    float z = 1.;
    float trk = 1.;
    float dspAmp = 0.1 + prm1*0.2; // shape modification over time
    
    // perturbate the cylinder with noise
    // the idea is like many ifs, start with a big warping
    // move and add a smaller warping to the last
    for(int i = 0; i < warpingIters; i++)
    {
		p += sin(p.zxy*0.75*trk + iTime*trk*.8)*dspAmp; // sin warped point (dspAmp => amplitude of warping)
        d -= abs(dot(cos(p), sin(p.yzx))*z); // noise dispalcment according to warped point
        z *= dispScaleInLoop;	// affect d over iterations 
        trk *= sinWarpScaleInLoop; // affect p over iterations
        p = p*m3;   // rot/scale of warped p for noise over iterations
    }
    d = abs(d + prm1*3.)+ prm1*.3 - 2.5 + bsMo.y; // final noise displacement
    // final distance
    // d is displacement (based on tunnel high freq), cl tunnel position (based on tunnel low freq)
    return vec2(d + cl*.2 + 0.25, cl);
}

@UNIFORMS

uniform(Volume Marcher) int(0:200:130) marchIters; // count iteration in volume marcher
uniform(Volume Marcher) float(0:1:0.99) breakCondition; // breakc alpha accum condition
uniform(Volume Marcher) vec2(0:5:2,3) density2; // second density used by ray step size
uniform(Volume Marcher) vec2(0,0.5:1,1.5:0.3,1.12) density;
uniform(Volume Marcher) vec2(0:0.5:0.09,0.3) clampedDistance; // clamp ray distance for control ray step size

uniform(Fog:0) float(checkbox:true) useFog; // use fog 
uniform(Fog:1:useFog==true) vec4(color:0.06,0.11,0.11, 0.1) fogColor; // fog color more influenced\n by alpha value

uniform(Clouds Color:0) float(0:10:0.6) startTunnelDist; // strat distand to tunnle for strat coloring,\n can save some tierations
uniform(Clouds Color:1) float(checkbox:true) useBaseColor; // base color step
uniform(Clouds Color:2:useBaseColor==true) vec3(color:5.,0.4,0.2) densityColor; // base tunnel color
uniform(Clouds Color:3:useBaseColor==true) vec4(0:1,1,1,6:0.1,0.4,0.5,1.8) baseColorVariation;
uniform(Clouds Color:4:useBaseColor==true) float(0:0.5:0.08) alphaAccumStep;
uniform(Clouds Color:10) float(checkbox:true) useHighDensity; // second step, apply high density
uniform(Clouds Color:20) float(checkbox:true) useColorLinStep; // third step, apply linear step
uniform(Clouds Color:21:useColorLinStep==true) vec3(-10:10:4,-2.5,2.3) linStepVars;
uniform(Clouds Color:30) float(checkbox:true) useFirstDiffusion; // 4th step, surface diffusion
uniform(Clouds Color:40) float(checkbox:true) useSecondDiffusion; // 5th step, differential diffusion (surface and inside)
uniform(Clouds Color:50) float(checkbox:true) useColorShadow; // 6th step, diffusion mix for cloud shadow
uniform(Clouds Color:51:useColorShadow==true) vec3(color:0.005,.045,.075) densityColor2; // color over tunnel axis

uniform(Camera:50) float(checkbox:false) useCamera; // use camera of noodlesplate (see top button for activate camera) :\n - left mouse => rotate\n - right mouse => zzom

@FRAGMENT

vec3 _globalP; // AIekick code used for calcDepth

vec4 render( in vec3 ro, in vec3 rd, float time )
{
	vec4 rez = vec4(0);
    const float ldst = 8.;
	vec3 lpos = vec3(disp(time + ldst)*0.5, time + ldst); // light pos / light path
	float t = 1.5; // start distance
	float fogT = 0.; // last fog value
	for(int i=0; i<marchIters; i++)
	{
        // alpha accumulation (volume marching)
        // for classical solid ray marching
       	// the distance to surface of the sdf is sufficent
        // but here the ray is maybe inside the sdf, so we use color accumulation threshold
        // the easy idea, is to iterate over the sdf, and blend the color along the ray
        // and when the color is opaque enough we stop
		if(rez.a > breakCondition) break; 

		vec3 pos = ro + t*rd; // surface pos (classic ray marching)
        vec2 mpv = map(pos);	// distance to tunnel // mpv.x high freq tunnel / mpv.y low freq tunnel
		float den = clamp(mpv.x-density.x,0.,1.)*density.y; // fist density
		float dn = clamp((mpv.x + density2.x),0.,density2.y);// second density
        
		vec4 col = vec4(0); // color init
        if (mpv.x > startTunnelDist) // he save some iterations by starting volume marching after distance to 0.6
        {
            // base color => use low freq tunnel => smooth
            // note the 0.08 for the alpha channel, this is used by breack condition
            // its like a constant alpha step over iterations. cant be too high for avoid banding
			if (useBaseColor>0.5)
				col = vec4(sin(densityColor + mpv.y*baseColorVariation.x + sin(pos.z*baseColorVariation.y)*baseColorVariation.z + baseColorVariation.w)*0.5 + 0.5,alphaAccumStep);
            // apply some density based on high freq tunnel
            // alpha channel impacted
            if (useHighDensity>0.5)
				col *= den*den*den;
            // after this line, only the color rgb is impacted without alpha channel
            // no change for break condition
            // color variation according to high freq
			if (useColorLinStep>0.5)
				col.rgb *= linstep(linStepVars.x,linStepVars.y, mpv.x)*linStepVars.z;
            // some light diffusion, he compare current pos density to coloud internal point density
            // similar tecnic used for egenerate subsufrace scattering
            // low cost volume diffusion color
            float dif = 0.0;
			if (useFirstDiffusion>0.5)
				clamp((den - map(pos+.8).x)/9., 0.001, 1. );
            // add another same way diff estimation
            if (useSecondDiffusion>0.5)
				dif += clamp((den - map(pos+.35).x)/2.5, 0.001, 1. );
            // give color variation according to density and diffusion
            // give the cloud shadow style
            if (useColorShadow>0.5)
				col.xyz *= den*(densityColor2 + 1.5*vec3(0.033,0.07,0.03)*dif);
        }
		
        // current fog based on second density
		float fogC = exp(t*0.2 - 2.2);
        // apply volumetric fog to color (difference between curent fog value and last fog value)
        // like that you can affect fog according density variation, let the fog penetrate clouds
		if (useFog > 0.5)
			col.rgba += fogColor*clamp(fogC-fogT, 0., 1.);
        // save of the current fog for differnetila fog ( at last line)
		fogT = fogC;
        // final color blending according to alpha channel
        rez = rez + col*(1. - rez.a);
        // use density for distance accumulation
        // used for fog and color diffusion for breack condition, but affect color accumulation
		t += clamp(0.5 - dn*dn*.05, clampedDistance.x, clampedDistance.y);
	}
	_globalP = ro + t * rd;
    // color result clamped
	return clamp(rez, 0.0, 1.0);
}

float getsat(vec3 c)
{
    float mi = min(min(c.x, c.y), c.z);
    float ma = max(max(c.x, c.y), c.z);
    return (ma - mi)/(ma+ 1e-7);
}

//from my "Will it blend" shader (https://www.shadertoy.com/view/lsdGzN)
vec3 iLerp(in vec3 a, in vec3 b, in float x)
{
    vec3 ic = mix(a, b, x) + vec3(1e-6,0.,0.);
    float sd = abs(getsat(ic) - mix(getsat(a), getsat(b), x));
    vec3 dir = normalize(vec3(2.*ic.x - ic.y - ic.z, 2.*ic.y - ic.x - ic.z, 2.*ic.z - ic.y - ic.x));
    float lgt = dot(vec3(1.0), ic);
    float ff = dot(dir, normalize(ic));
    ic += 1.5*dir*sd*ff*lgt;
    return clamp(ic,0.,1.);
}

@UNIFORMS

uniform(sdf) float(-1:1:0.4) mixerOverTime; // shapoe in map fucntion, and color after render
 
uniform(PostProcessing:-1) text( R   G   B) swizzle;
uniform(PostProcessing:0) vec3(radio:0) yellowColorR;
uniform(PostProcessing:1) vec3(radio:1) yellowColorG;
uniform(PostProcessing:2) vec3(radio:2) yellowColorB;
uniform(PostProcessing:3) vec3(radio:2) blueColorR;
uniform(PostProcessing:4) vec3(radio:1) blueColorG;
uniform(PostProcessing:5) vec3(radio:0) blueColorB;
	
uniform(PostProcessing:9) float(checkbox:true) useGammaCorrect;
uniform(PostProcessing:10) float(checkbox:true) useVignette;

@FRAGMENT

#include "space3d.glsl"

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{	
	_globalP = vec3(0.0); // add by aiekick for calcdepth
	
    // standard uv 0->1 (used for vignette)
	vec2 q = fragCoord.xy/iResolution.xy;
    // central uv -1->1
    vec2 p = (gl_FragCoord.xy - 0.5*iResolution.xy)/iResolution.y;
    // mouse centered and normalized -1->1
    bsMo = (iMouse.xy - 0.5*iResolution.xy)/iResolution.y;
    
    float time = iTime*3.;
    // eye point
    vec3 ro = vec3(0,0,time);
    // eye base path over time
    ro += vec3(sin(iTime)*0.5,sin(iTime*1.)*0.,0);
        
    float dspAmp = .85;
    // add a bit of the path used by sdf 
    // the goal is to follow shape but not go far or inside the volume i thinck :)
    ro.xy += disp(ro.z)*dspAmp; 
    float tgtDst = 3.5;
    
	// camera setup
    vec3 target = normalize(ro - vec3(disp(time + tgtDst)*dspAmp, time + tgtDst));
    // affect eye x moving acording to mouse
    ro.x -= bsMo.x*2.;
    vec3 rightdir = normalize(cross(target, vec3(0,1,0)));
    vec3 updir = normalize(cross(rightdir, target));
    rightdir = normalize(cross(updir, target));
	vec3 rd=normalize((p.x*rightdir + p.y*updir)*1. - target);
    // apply some rotation of the ray direction affecte by mouse also
    rd.xy *= rot(-disp(time + 3.5).x*0.2 + bsMo.x);
	
	if (useCamera > 0.5)
	{
		ro = getRayOrigin();
		rd = getRayDirection();
	}
	
    // prm1 is initilaized here and used in mpa for mis shape and color over time
    // blue and yellow based style
    prm1 = smoothstep(-mixerOverTime, mixerOverTime,sin(iTime*0.3));
    // render scene color
	vec4 scn = render(ro, rd, time);
	
	// blue and yellow color sheme is defined here
    // the shape was mixed in map function
    // here we just do color
    // the blue sheme is just obtained by swizzle normal color
    // yellow based => scn.rgb
    // blue based => scn.bgr (low cost tuning) i prefer the yellow :)
    vec3 col = scn.bgr;
    
	vec3 _baseColor = scn.rgb;
	if (yellowColorR.x > 0.5) _baseColor.r = col.r;
    if (yellowColorR.y > 0.5) _baseColor.r = col.g;
    if (yellowColorR.z > 0.5) _baseColor.r = col.b;
    if (yellowColorG.x > 0.5) _baseColor.g = col.r;
    if (yellowColorG.y > 0.5) _baseColor.g = col.g;
    if (yellowColorG.z > 0.5) _baseColor.g = col.b;
    if (yellowColorB.x > 0.5) _baseColor.b = col.r;
    if (yellowColorB.y > 0.5) _baseColor.b = col.g;
    if (yellowColorB.z > 0.5) _baseColor.b = col.b;
	
	vec3 _endColor = scn.rgb;
	if (blueColorR.x > 0.5) _endColor.r = col.r;
    if (blueColorR.y > 0.5) _endColor.r = col.g;
    if (blueColorR.z > 0.5) _endColor.r = col.b;
    if (blueColorG.x > 0.5) _endColor.g = col.r;
    if (blueColorG.y > 0.5) _endColor.g = col.g;
    if (blueColorG.z > 0.5) _endColor.g = col.b;
    if (blueColorB.x > 0.5) _endColor.b = col.r;
    if (blueColorB.y > 0.5) _endColor.b = col.g;
    if (blueColorB.z > 0.5) _endColor.b = col.b;
		
    col = iLerp(_baseColor, _endColor, clamp(1.-prm1,0.05,1.));
    //col = iLerp(col.bgr, col.rgb, clamp(1.-prm1,0.05,1.));
    
    // gamma correction
	if (useGammaCorrect > 0.5)
		col = pow(col, vec3(.55,0.65,0.6))*vec3(1.,.97,.9);

    // vignette
    if (useVignette > 0.5)
		col *= pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.12)*0.7+0.3; //Vign
    
	fragColor = vec4( col, 1.0 );
}

void main(void)
{
	mainImage(fragColor, gl_FragCoord.xy);
	fragColor.a = 1.0;
}
