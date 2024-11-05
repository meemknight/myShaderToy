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

using uniform = GLint;

struct Uniform
{
	std::string name;
	GLint size = 0;
	GLenum type = 0;
	GLint location = -1;

	union
	{
		glm::vec4 vec4;
		glm::ivec4 ivec4;
		glm::uvec4 uvec4;
	}data = {};
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

	void bindAndSendUniforms();

	struct
	{
		uniform iResolution = -1;        // viewport resolution (in pixels)		->	vec3      
		uniform iTime = -1;              // shader playback time (in seconds)		->	float     
		uniform iTimeDelta = -1;         // render time (in seconds)				->	float     
		uniform iFrameRate = -1;         // shader frame rate						->	float     
		uniform iFrame = -1;             // shader playback frame					->	int       
		//uniform iChannelTime[4];       // channel playback time (in seconds)	->	float     
		//uniform iChannelResolution[4]; // channel resolution (in pixels)		->	vec3      
		uniform iMouse = -1;             // mouse pixel coords. xy: current (if MLB down), zw: click ->	vec4      
		//uniform iChannel0..3;          // input channel. XX = 2D/Cube			->	samplerXX 
		//uniform iDate;                 // (year, month, day, time in seconds)	->	vec4      
		//uniform iSampleRate;           // sound sample rate (i.e., 44100)		->	float     


	}specialUniforms;

};