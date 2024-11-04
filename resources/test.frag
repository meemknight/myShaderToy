#version 330 core

layout (location = 0) out vec4 color;

uniform vec3 u_color;

in vec2 v_position;


void main()
{
	
	color = vec4((v_position.x + 1)/2.f,1,1,1) + vec4(u_color, 0);

}