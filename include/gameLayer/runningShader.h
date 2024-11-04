#pragma once

#include <shader.h>
#include <gl2d/gl2d.h>
#include <string>
#include <TextEditor.h>


struct Renderer2D
{

	GLuint vao;
	GLuint buffer;

	void init();
	void render();
	
};

struct Uniform
{
	std::string name;
	GLint size = 0;
	GLenum type = 0;
	GLint location = -1;
};

struct RunningShader
{

	Shader shader;
	gl2d::FrameBuffer frameBuffer;
	std::string name;
	std::vector<Uniform> uniforms;
	TextEditor textEditor;

	int w = 1; 
	int h = 1;

	bool init(const char *name);

	void updateSize();

	void displayImgui();

	void displaySettings();

	void displayPreview();

	bool reload();
};