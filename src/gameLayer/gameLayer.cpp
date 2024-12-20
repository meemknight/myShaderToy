#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <runningShader.h>


RunningShader runningShader;
Renderer2D renderer;

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();


	runningShader.init(RESOURCES_PATH "mario.frag");
	renderer.init();
	renderer.loadDefaultTextures();

	return true;
}


//IMPORTANT NOTICE, IF YOU WANT TO SHIP THE GAME TO ANOTHER PC READ THE README.MD IN THE GITHUB
//https://github.com/meemknight/cmakeSetup
//OR THE INSTRUCTION IN THE CMAKE FILE.
//YOU HAVE TO CHANGE A FLAG IN THE CMAKE SO THAT RESOURCES_PATH POINTS TO RELATIVE PATHS
//BECAUSE OF SOME CMAKE PROGBLMS, RESOURCES_PATH IS SET TO BE ABSOLUTE DURING PRODUCTION FOR MAKING IT EASIER.

bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

#pragma endregion

#pragma region web camera

	bool shouldCameraRun = 0;

	for (int i = 0; i < 4; i++)
	{
		if (runningShader.mainShader.inputBuffers[i].t.id == renderer.webCamera.t.id)
		{
			shouldCameraRun = true;
			break;
		}
	}

	for (int j = 0; j < 4; j++)
	for (int i = 0; i < 4; i++)
	{
		if (runningShader.shaderBuffers[j].inputBuffers[i].t.id == renderer.webCamera.t.id)
		{
			shouldCameraRun = true;
			break;
		}
	}

	if (!shouldCameraRun)
	{
		renderer.webCamera.deinitCapture();
	}
	else
	{
		renderer.webCamera.startCapture();
		renderer.webCamera.doCapture();
	}

#pragma endregion


#pragma region render

	runningShader.updateSize();
	runningShader.updateSimulation(deltaTime);

	for (int i = 0; i < 4; i++)
	{
		auto &b = runningShader.shaderBuffers[i];

		if (b.shader.id)
		{
			b.bindAndSendUniforms(renderer, runningShader);
			glBindFramebuffer(GL_FRAMEBUFFER, b.frameBuffer.fbo);
			glViewport(0, 0, runningShader.w, runningShader.h);
			renderer.render();
		};

	}

	runningShader.mainShader.bindAndSendUniforms(renderer, runningShader);
	glBindFramebuffer(GL_FRAMEBUFFER, runningShader.mainShader.frameBuffer.fbo);
	glViewport(0, 0, runningShader.w, runningShader.h);
	renderer.render();



	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	runningShader.displayImgui(renderer);

#pragma endregion

	
	glViewport(0, 0, w, h);

	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{


}
