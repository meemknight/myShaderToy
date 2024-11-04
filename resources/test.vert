#version 330 core

layout (location = 0) in vec2 p;

out vec2 v_position;

void main()
{
	gl_Position = vec4(p, 0, 1);
	v_position = p;
}