#include "runningShader.h"
#include "imgui.h"
#include <imgui_internal.h>
#include <glm/vec2.hpp>
#include <iostream>
#include <platform/platformInput.h>
#include <fstream>

bool RunningShader::init(const char *name)
{

	this->name = name;

	
	frameBuffer.create(1, 1);

	return reload();

}

void RunningShader::updateSize()
{
	frameBuffer.resize(w, h);
	frameBuffer.clear();
}


void RunningShader::displayImgui()
{
	
	displayPreview();

	displaySettings();

	std::string textEditorName = this->name + " edit";
	ImGui::Begin(textEditorName.c_str());
	textEditor.Render(textEditorName.c_str());
	ImGui::End();
}

void RunningShader::displaySettings()
{
	std::string windowName = name + " settings";

	ImGui::Begin(windowName.c_str());

	ImGui::Text("Uniforms: ");
	ImGui::Separator();
	ImGui::NewLine();

	for (int i = 0; i < uniforms.size(); i++)
	{
		auto &u = uniforms[i];

		if (u.type == GL_FLOAT_VEC3)
		{

			static glm::vec3 temp = {};
			ImGui::ColorEdit3(u.name.c_str(), &temp[0]);

		}

	}

	ImGui::Separator();
	ImGui::NewLine();

	ImGui::End();
}

void RunningShader::displayPreview()
{
	std::string resultWindowName = name + " view";

	bool isOpen = 1;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 1.0f));
	ImGui::SetNextWindowSize({200,200}, ImGuiCond_Once);
	ImGui::Begin(resultWindowName.c_str(),
		&isOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav);

	auto windowSize = ImGui::GetWindowSize();

	w = windowSize.x;
	h = windowSize.y;


	//mouse pos and focus
	auto windowPos = ImGui::GetWindowPos();

	glm::vec2 windowInput = {};
	{

		ImVec2 globalMousePos = {};
		{
			ImGuiContext *g = ImGui::GetCurrentContext();
			globalMousePos = g->IO.MousePos;
		}

		windowInput.x = globalMousePos.x;
		windowInput.y = globalMousePos.y;

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		windowInput.x -= windowPos.x + vMin.x;
		windowInput.y -= windowPos.y + vMin.y;


		//https://github.com/ocornut/imgui/issues/5882
		ImGuiViewport *viewPort = ImGui::GetWindowViewport();
		auto io = ImGui::GetIO();

		//if (viewPort->PlatformUserData)
		//{
		//	windowInput.hasFocus = ImGui::IsWindowFocused()
		//		&& ImGui::GetPlatformIO().Platform_GetWindowFocus(viewPort) && !io.AppFocusLost;
		//}
		//else
		//{
		//	windowInput.hasFocus = 0;
		//}

		//windowInput.hasFocus = windowInput.hasFocus && !io.AppFocusLost;
	}


	auto s = ImGui::GetContentRegionMax();

	//todo try set borders here at 0,0, easiest thing to do probably
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	//ImGui::Image((void *)c.second.requestedContainerInfo.requestedFBO.texture, s, {0, 1}, {1, 0},
	//	{1,1,1,1}, {0,0,0,0});

	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 maxPos = {ImGui::GetWindowPos().x + ImGui::GetWindowSize().x,
		ImGui::GetWindowPos().y + ImGui::GetWindowSize().y};
	unsigned int texId = frameBuffer.texture.id;

	ImGui::GetWindowDrawList()->AddImage(
		(void *)texId,
		ImVec2(pos.x, pos.y),
		ImVec2(maxPos),
		ImVec2(0, 1), ImVec2(1, 0)
	);

	ImGui::PopStyleVar();

	ImGui::End();

	ImGui::PopStyleColor();
}

bool RunningShader::reload()
{
	uniforms.clear();

	shader.clear();

	shader.loadShaderProgramFromFile(
		(this->name + ".vert").c_str(),
		(this->name + ".frag").c_str()
	);

	if (!shader.id)
	{
		return 0;
	}

	//text editor
	{
		std::ifstream file;
		file.open(this->name + ".frag");

		if (!file.is_open())
		{
			std::cout << "Error openning file: " << this->name << "\n";
			return 0;
		}

		GLint size = 0;
		file.seekg(0, file.end);
		size = file.tellg();
		file.seekg(0, file.beg);

		char *fileContent = new char[size + 1] {};

		file.read(fileContent, size);

		file.close();

		textEditor.SetText(fileContent);
		textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());

		delete[] fileContent;
	}

	GLint numUniforms = 0;
	glGetProgramiv(shader.id, GL_ACTIVE_UNIFORMS, &numUniforms);

	for (int i = 0; i < numUniforms; ++i)
	{
		char name[257] = {};
		GLsizei length = 0;
		GLint size = 0;
		GLenum type = 0;
		glGetActiveUniform(shader.id, i, sizeof(name), &length, &size, &type, name);
		GLint location = glGetUniformLocation(shader.id, name);
		
		Uniform uniform;
		uniform.location = location;
		uniform.name = name;
		uniform.type = type;
		uniform.size = size;

		std::cout << name << " " << type << "\n";

		uniforms.push_back(uniform);
	}


	return true;
}



float data[] = 
{
	-1,1,	-1,-1,	1,1,
	1,1,	-1,-1,	1,-1
};

void Renderer2D::init()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, 0);

	glBindVertexArray(0);
}

void Renderer2D::render()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

