#version 330 core

layout (location = 0) in vec2 p;

out vec2 v_position;

uniform mat4 iProjecton;
uniform mat4 iView;

void main()
{
	v_position = p;
	gl_Position = iProjecton * iView * vec4(p, 0, 1);
}