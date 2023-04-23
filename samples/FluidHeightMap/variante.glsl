@NOTE SECTION(points)

in this section we have only points

@UNIFORMS SECTION(points)

uniform float(0.0:20.0:3.0) pointSize;

@VERTEX SECTION(points) DISPLAY(POINTS)

	gl_PointSize = pointSize;
	
	float width = 0., index = 0.;
	width = floor(sqrt(maxpoints));
	index = vertexId;
	
	vec2 id = vec2(0);
	id.x = mod(index, width);
	id.y = floor(index / width);
	
	p.xz += id;
	
	vec4 chan = getTexture(id / width);
	
	p.y = chan.r * altitude;
		
	p.x -= width * 0.5;
	p.z -= width * 0.5;
	
@NOTE SECTION(quad)

in this section we have only quads (2 triangles )
with many settings
Happy tuning :)

@VERTEX SECTION(quad) DISPLAY(TRIANGLES) 

	float width = 0., index = 0.;
	width = floor(sqrt(maxpoints/6.));
	
	vec4 pi = getQuad(vertexId);
	p = pi.xyz;
	index = pi.w;
	
	p.xz *= 0.5;
	
	vec2 id = vec2(0);
	id.x = mod(index, width);
	id.y = floor(index / width);
	
	vec4 chan = getTexture(id / width);
	
@CONFIG_START Scaled_1
	p.y = 0.;
	p.xz *= chan.r*2.0-1.0;
@CONFIG_END

@CONFIG_START Scaled_2
	p.y = 0.;
	p.xz *= clamp(chan.r+0.5,0.,1.);
@CONFIG_END

@CONFIG_START Elevated
	p.y = chan.r * altitude ;
@CONFIG_END
	
@CONFIG_START Scaled_1_Elevated
	p.y = chan.r * altitude;
	p.xz *= chan.r*2.0-1.0;
@CONFIG_END

@CONFIG_START Scaled_2_Elevated
	p.y = chan.r * altitude;
	p.xz *= clamp(chan.r+0.5,0.,1.);
@CONFIG_END

@CONFIG_START Rot_XZ
	float a = (chan.r*2.0-1.0) * 180.;
	p *= RotY(45.) * RotZ(a) * RotY(-45.);
@CONFIG_END

@CONFIG_START Rot_XZ_Scaled
	float a = (chan.r*2.0-1.0) * 180.;
	p.xz *= clamp(chan.r+0.5,0.,1.);
	p *= RotY(45.) * RotZ(a) * RotY(-45.);
@CONFIG_END

	p.xz += id;
	
	p.x -= width * 0.5;
	p.z -= width * 0.5;
	
	
@NOTE SECTION(cube)

in this section we have only cubes
with many settings
Happy tuning :)

@UNIFORMS SECTION(cube)

uniform float(0.0:360.0:360.0) uAngle;
uniform float(checkbox) uUsePyramide;
uniform float(0.0:10.0:2.0) uBottomScale;
uniform float(0.0:10.0:0.1) uTopScale;

@VERTEX SECTION(cube) DISPLAY(TRIANGLES)

	float width = 0., index = 0.;
	width = floor(sqrt(maxpoints/(6.*6.)));
	
	cubeStruct pi = getCube(vertexId);
	p = pi.p;
	index = pi.i;
		
	p.xz *= 0.5;
	
	vec2 id = vec2(0);
	id.x = mod(index, width);
	id.y = floor(index / width);
	
	vec4 chan = getTexture(id / width);
	
	p.y = p.y * chan.r * altitude + 0.1 * sign(p.y);
	
@CONFIG_START No_Config
	
@CONFIG_END

@CONFIG_START Scaled_1
	p.xz *= chan.r*2.0-1.0;
@CONFIG_END

@CONFIG_START Scaled_2
	p.xz *= clamp(chan.r+0.5,0.,1.);
@CONFIG_END

@CONFIG_START Inv_Scaled_2
	p.xz *= 1.8-clamp(chan.r+0.5,0.,1.);
@CONFIG_END

	if (uUsePyramide > 0.5)
		if (p.y < 0.)
			p.xz *= uBottomScale;
		else if (p.y > 0.)
			p.xz *= uTopScale;

	vec3 pos = p;
	
	p.xz += id;
	
	//p.y += width;
	//p *= RotZ(id.x / (width*2.) * uAngle);
	//p.y -= width;
	
	p.x -= width * 0.5;
	p.z -= width * 0.5;
	
	pos_nor pn = getPos(p.xz, 10.);
	p.y += pn.p.y;
	
	vec3 n = pn.n;
	vec3 u = vec3(0,1,0);
	mat3 m = lookat(n, u);
	//p = p + (pos * m - pos);
	//p = p + pos * lookat(pn.n, vec3(1e-4,1,1e-4));
	
	//p *= Rot(chan.rgb*180.*0.1);
	//p *= RotY(id.y*0.5);
	
	//p *= Rot(chan.rgb * 10. + id.x/width);
	
	chan.rgb = chan.rgb * 0.8 + pi.n * 0.2;//reflect(chan.rgb, pi.n);

	
