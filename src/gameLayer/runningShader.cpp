#include "runningShader.h"
#include "imgui.h"
#include <imgui_internal.h>
#include <glm/vec2.hpp>
#include <iostream>
#include <platform/platformInput.h>
#include <fstream>
#include <glslGrammar.h>

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

		if (u.type == GL_FLOAT)
		{
			ImGui::DragFloat(u.name.c_str(), &u.data.vec4[0]);
		}
		else if (u.type == GL_FLOAT_VEC2)
		{
			ImGui::DragFloat2(u.name.c_str(), &u.data.vec4[0]);
		}
		else if (u.type == GL_FLOAT_VEC3)
		{
			ImGui::DragFloat3(u.name.c_str(), &u.data.vec4[0]);
			//ImGui::ColorEdit3(u.name.c_str(), &u.data.vec4[0]);
		}else if (u.type == GL_FLOAT_VEC4)
		{
			ImGui::DragFloat4(u.name.c_str(), &u.data.vec4[0]);
			//ImGui::ColorEdit4(u.name.c_str(), &u.data.vec4[0]);
		}

		else if (u.type == GL_INT)
		{
			ImGui::DragInt(u.name.c_str(), &u.data.ivec4[0]);
		}
		else if (u.type == GL_INT_VEC2)
		{
			ImGui::DragInt2(u.name.c_str(), &u.data.ivec4[0]);
		}
		else if (u.type == GL_INT_VEC3)
		{
			ImGui::DragInt3(u.name.c_str(), &u.data.ivec4[0]);
		}
		else if (u.type == GL_INT_VEC4)
		{
			ImGui::DragInt4(u.name.c_str(), &u.data.ivec4[0]);
		}

		else if (u.type == GL_UNSIGNED_INT)
		{
			ImGui::DragScalar(u.name.c_str(), ImGuiDataType_U32, &u.data.uvec4[0]);
		}
		else if (u.type == GL_UNSIGNED_INT_VEC2)
		{
			ImGui::DragScalarN(u.name.c_str(), ImGuiDataType_U32, &u.data.uvec4[0], 2);
		}
		else if (u.type == GL_UNSIGNED_INT_VEC3)
		{
			ImGui::DragScalarN(u.name.c_str(), ImGuiDataType_U32, &u.data.uvec4[0], 3);
		}
		else if (u.type == GL_UNSIGNED_INT_VEC4)
		{
			ImGui::DragScalarN(u.name.c_str(), ImGuiDataType_U32, &u.data.uvec4[0], 4);
		}

		else if (u.type == GL_BOOL)
		{
			ImGui::Checkbox(u.name.c_str(), (bool *)&u.data.ivec4[0]);
		}
		else if (u.type == GL_BOOL_VEC2)
		{
			ImGui::Checkbox(("##bool2" + u.name).c_str(), (bool *)&u.data.ivec4[0]);
			ImGui::SameLine();
			ImGui::Checkbox(u.name.c_str(), (bool *)&u.data.ivec4[1]);
		}
		else if (u.type == GL_BOOL_VEC3)
		{
			ImGui::Checkbox(("##bool3" + u.name).c_str(), (bool *)&u.data.ivec4[0]);
			ImGui::SameLine(); ImGui::Checkbox(("##bool2" + u.name).c_str(), (bool *)&u.data.ivec4[1]);
			ImGui::SameLine(); ImGui::Checkbox(u.name.c_str(), (bool *)&u.data.ivec4[2]);
		}
		else if (u.type == GL_BOOL_VEC4)
		{
			ImGui::Checkbox(("##bool4" + u.name).c_str(), (bool *)&u.data.ivec4[0]);
			ImGui::SameLine(); ImGui::Checkbox(("##bool2" + u.name).c_str(), (bool *)&u.data.ivec4[1]);
			ImGui::SameLine(); ImGui::Checkbox(("##bool3" + u.name).c_str(), (bool *)&u.data.ivec4[2]);
			ImGui::SameLine(); ImGui::Checkbox(u.name.c_str(), (bool *)&u.data.ivec4[3]);
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

const char *uniformNames[] = {
	"iResolution",
	"iTime",
	"iTimeDelta",
	"iFrameRate",
	"iFrame",
	"iMouse",
};

std::string readFileToString(const char *filePath)
{
	std::ifstream file(filePath, std::ios::in | std::ios::binary);
	if (!file)
	{
		//todo error out
		return "";
	}

	// Move to the end to get the file size, then back to the beginning
	file.seekg(0, std::ios::end);
	std::string content;
	content.resize(file.tellg());
	file.seekg(0, std::ios::beg);

	// Read file into the string
	file.read(&content[0], content.size());
	file.close();

	return content;
}

bool RunningShader::reload()
{
	uniforms.clear();
	specialUniforms = {};

	shader.clear();


	//get the data.
	{
		std::string vertexData = readFileToString((this->name + ".vert").c_str());
		std::string fragmentData = readFileToString((this->name + ".frag").c_str());


		auto rez = tokenizeGLSL(fragmentData.c_str());


		//for (auto &i : rez)
		//{
		//	std::string t(vertexData.data() + i.start, vertexData.data() + i.end);
		//	std::cout << i.type << " ->  " << t << "\n";
		//}


		if (!hasVersion(rez, fragmentData.c_str()))
		{
			std::cout << "No version!";

			fragmentData = "#version 330 core\n" + fragmentData;
		}
		else
		{
			std::cout << "has version!";
		}

		//add main function
		if (true)
		{
		
			fragmentData += 
				R"(
				void main()
				{
				vec4 outColor = vec4(0, 0, 0, 0);
				mainImage(outColor, gl_FragCoord.xy);
				out_color = outColor;
				}
				)";
		}


		shader.loadShaderProgramFromData(
			vertexData.c_str(), fragmentData.c_str()
		);

	}



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
		
		bool notGood = 0;

		for (int j = 0; j < sizeof(uniformNames) / sizeof(uniformNames[0]); j++)
		{

			if (strstr(name, uniformNames[j]) == 0)
			{
				notGood = true;
				break;
			}

		}

		if (!notGood)
		{
			Uniform uniform;
			uniform.location = location;
			uniform.name = name;
			uniform.type = type;
			uniform.size = size;

			std::cout << name << " " << type << "\n";

			uniforms.push_back(uniform);
		};

	}

	specialUniforms.iResolution = glGetUniformLocation(shader.id, "iResolution");
	specialUniforms.iTime = glGetUniformLocation(shader.id, "iTime");
	specialUniforms.iTimeDelta = glGetUniformLocation(shader.id, "iTimeDelta");
	specialUniforms.iFrameRate = glGetUniformLocation(shader.id, "iFrameRate");
	specialUniforms.iFrame = glGetUniformLocation(shader.id, "iFrame");
	specialUniforms.iMouse = glGetUniformLocation(shader.id, "iMouse");
	
	return true;
}

void RunningShader::bindAndSendUniforms()
{
	
	shader.bind();

	for (int i = 0; i < uniforms.size(); i++)
	{
		auto &u = uniforms[i];

		if (u.type == GL_FLOAT)
		{
			glUniform1f(u.location, u.data.vec4.x);
		}
		else if (u.type == GL_FLOAT_VEC2)
		{
			glUniform2fv(u.location, 1, &u.data.vec4.x);
		}
		else if (u.type == GL_FLOAT_VEC3)
		{
			glUniform3fv(u.location, 1, &u.data.vec4.x);
		}
		else if (u.type == GL_FLOAT_VEC4)
		{
			glUniform4fv(u.location, 1, &u.data.vec4.x);
		}

		else if (u.type == GL_INT)
		{
			glUniform1i(u.location, u.data.ivec4.x);
		}
		else if (u.type == GL_INT_VEC2)
		{
			glUniform2iv(u.location, 1, &u.data.ivec4.x);
		}
		else if (u.type == GL_INT_VEC3)
		{
			glUniform3iv(u.location, 1, &u.data.ivec4.x);
		}
		else if (u.type == GL_INT_VEC4)
		{
			glUniform4iv(u.location, 1, &u.data.ivec4.x);
		}

		else if (u.type == GL_UNSIGNED_INT)
		{
			glUniform1ui(u.location, u.data.uvec4.x);
		}
		else if (u.type == GL_UNSIGNED_INT_VEC2)
		{
			glUniform2uiv(u.location, 1, &u.data.uvec4.x);
		}
		else if (u.type == GL_UNSIGNED_INT_VEC3)
		{
			glUniform3uiv(u.location, 1, &u.data.uvec4.x);
		}
		else if (u.type == GL_UNSIGNED_INT_VEC4)
		{
			glUniform4uiv(u.location, 1, &u.data.uvec4.x);
		}

		else if (u.type == GL_BOOL)
		{
			glUniform1i(u.location, u.data.ivec4.x);
		}
		else if (u.type == GL_BOOL_VEC2)
		{
			glUniform2iv(u.location, 1, &u.data.ivec4.x);
		}
		else if (u.type == GL_BOOL_VEC3)
		{
			glUniform3iv(u.location, 1, &u.data.ivec4.x);
		}
		else if (u.type == GL_BOOL_VEC4)
		{
			glUniform4iv(u.location, 1, &u.data.ivec4.x);
		}

	}

	//special uniforms
	glUniform3f(specialUniforms.iResolution, w, h, 0);
	glUniform1f(specialUniforms.iTime, 0); //todo
	glUniform1f(specialUniforms.iTimeDelta, 0); //todo
	glUniform1f(specialUniforms.iFrameRate, 0); //todo
	glUniform1i(specialUniforms.iFrame, 0); //todo
	glUniform4f(specialUniforms.iMouse, 0,0,0,0); //todo

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

