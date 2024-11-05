#version 330 core

layout (location = 0) out vec4 out_color;

uniform vec3 u_color;

uniform bvec3 u_test;

uniform vec3 iResolution;

#define time 0.


float stripe(vec2 uv) {
	return cos(uv.x*20.-time+uv.y*-30.);
}

float glass(vec2 uv) {
	return cos(dot(uv.xy, vec2(12.41234,2442.123))*cos(uv.y));
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{

	vec2 uv = fragCoord.xy / iResolution.xy;
	float a = iResolution.x/iResolution.y;
	uv.x *= a;
	

	float g = stripe(uv);

	
	vec3 col = vec3(smoothstep(0., .2, g));

	col.r = .8;
	col /= (pow(glass(vec2(uv.x*30., uv.y)),2.))+.5;
	
  
	//Mask sides
	col *= smoothstep(.12, .0, abs(uv.x - .5*a));

	//Mask top and bottom
	col *= smoothstep(.33, .30, abs(uv.y - .5));

	if (uv.y > .80 && uv.y < .94 || uv.y < .2 && uv.y >.06) {
	   col = vec3(smoothstep(.13, .0, abs(uv.x - .5*a)));
	
	}

	if (uv.y > .77 && uv.y < .87 || uv.y < .23 && uv.y >.13) {
	   col = vec3(smoothstep(.15, .0, abs(uv.x - .5*a)));
		
	}

	fragColor = vec4(col,1.0);
}