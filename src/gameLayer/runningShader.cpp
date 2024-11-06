#include "runningShader.h"
#include "imgui.h"
#include <imgui_internal.h>
#include <glm/vec2.hpp>
#include <iostream>
#include <platform/platformInput.h>
#include <fstream>
#include <glslGrammar.h>
#include <imguiStuff.h>
#include <escapi/escapi.h>

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


void RunningShader::displayImgui(Renderer2D &renderer)
{
	
	displayPreview();

	displaySettings(renderer);

	std::string textEditorName = this->name + " edit";
	ImGui::Begin(textEditorName.c_str());
	textEditor.Render(textEditorName.c_str());
	ImGui::End();
}

void RunningShader::displaySettings(Renderer2D &renderer)
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
			//ImGui::DragFloat3(u.name.c_str(), &u.data.vec4[0]);
			ImGui::ColorEdit3(u.name.c_str(), &u.data.vec4[0]);
		}else if (u.type == GL_FLOAT_VEC4)
		{
			//ImGui::DragFloat4(u.name.c_str(), &u.data.vec4[0]);
			ImGui::ColorEdit4(u.name.c_str(), &u.data.vec4[0]);
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


	ImGui::Text("Buffers: ");

	//if()
	for (int i = 0; i < 4; i++)
	{
		auto b = &inputBuffers[i];

		if (b->t.id == 0)
		{
			b->t.id = renderer.blackTexture.id;
		}
	}

	if (renderer.defaultTextures.size())
	{

		auto drawButton = [&](int index)
		{

			auto id = inputBuffers[index].t.id;

			if (!id) { id = renderer.blackTexture.id; }

			const char *names[4] = {
				"iChannel0",
				"iChannel1",
				"iChannel2",
				"iChannel3"
			};

			bool pressedCog = 0;
			bool pressedX = 0;

			if (drawImageButtonWithLabelAndCog((ImTextureID)id,
				names[index], {140, 140}, pressedCog, pressedX
				))
			{
				selectedInputBuffer = index;
				inputSelectorOpen = true;
				ImGui::OpenPopup("Select Input Popup");
			}

			if (pressedX)
			{
				inputBuffers[index].t.id = renderer.blackTexture.id;
			}

		};
		
		drawButton(0);
		ImGui::SameLine();
		drawButton(1);
		ImGui::SameLine();
		drawButton(2);
		ImGui::SameLine();
		drawButton(3);

		//ImGui::SetNextWindowSize(ImVec2(400, 300));

		if (ImGui::BeginPopupModal("Select Input Popup", &inputSelectorOpen))
		{

			if (!inputSelectorOpen) { ImGui::CloseCurrentPopup(); }

			// Begin vertical tab bar
			if (ImGui::BeginTabBar("TabBar"))
			{

				// "Misc" tab
				if (ImGui::BeginTabItem("Misc"))
				{
					ImGui::NewLine();

					if (ImGui::Button("Web Camera"))
					{
						inputBuffers[selectedInputBuffer].t = renderer.webCamera.t;

						ImGui::CloseCurrentPopup(); // Close the popup when this button is pressed
					}
					ImGui::EndTabItem();
				}

				// "Textures" tab
				if (ImGui::BeginTabItem("Textures"))
				{
					ImGui::NewLine();

					for (int i = 0; i < renderer.defaultTextures.size(); i++)
					{

						if (ImGui::ImageButton((void *)renderer.defaultTextures[i].t.id,
							{140, 140}, {0,1}, {1,0}
							))
						{
							inputBuffers[selectedInputBuffer].t = renderer.defaultTextures[i].t;
							ImGui::CloseCurrentPopup();
						}

						if (!(i % 4 == 3))
						{
							ImGui::SameLine();
						}

					}
					
					ImGui::EndTabItem();
				}

				// "Cubemaps" tab
				if (ImGui::BeginTabItem("Cubemaps"))
				{
					ImGui::NewLine();

					if (ImGui::Button("Cubemaps Button"))
					{
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndTabItem();
				}

				// "Volumes" tab
				if (ImGui::BeginTabItem("Volumes"))
				{
					ImGui::NewLine();
					
					if (ImGui::Button("Volumes Button"))
					{
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}


			ImGui::EndPopup();
		}

	}



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

		currentMousePos = windowInput;
		currentMousePos.y = h - (currentMousePos.y);

		focused = 1;
		if(currentMousePos.x < 0 || currentMousePos.y < 0 || currentMousePos.x > w ||
			currentMousePos.y > h){ focused = 0; }

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
	"iChannel0",
	"iChannel1",
	"iChannel2",
	"iChannel3",
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

		std::string newFragment = fragmentData;
		std::string newFragmentStart = "";
		int versionPos = 0;

		if (!hasVersion(rez, fragmentData.c_str(), &versionPos))
		{
			std::cout << "No version!";

			newFragmentStart += "#version 330 core\n";
		}
		else
		{
			std::cout << "has version!";
		}

		std::string mainColorOutputName;
		if (!hasMainColorOutput(rez, fragmentData.c_str(), &mainColorOutputName))
		{
			std::cout << "No main color output!";

			mainColorOutputName = "out_color";
			newFragmentStart += "\nlayout (location = 0) out vec4 out_color;\n";
			
		}
		else
		{
			std::cout << "has main color output! " << mainColorOutputName << "\n";
		}

	#pragma region add uniforms
		//uniform iResolution = -1;        // viewport resolution (in pixels)		->	vec3      
		//uniform iTime = -1;              // shader playback time (in seconds)		->	float     
		//uniform iTimeDelta = -1;         // render time (in seconds)				->	float     
		//uniform iFrameRate = -1;         // shader frame rate						->	float     
		//uniform iFrame = -1;             // shader playback frame					->	int       
		//uniform iMouse = -1;             // mouse pixel coords. xy: current (if MLB down), zw: click -> vec4    

		if (!hasUniform(rez, fragmentData.c_str(), "iResolution", "vec3"))
		{
			newFragmentStart += "\nuniform vec3 iResolution;\n";
		}

		if (!hasUniform(rez, fragmentData.c_str(), "iTime", "float"))
		{
			newFragmentStart += "\nuniform float iTime;\n";
		}

		if (!hasUniform(rez, fragmentData.c_str(), "iTimeDelta", "float"))
		{
			newFragmentStart += "\nuniform float iTimeDelta;\n";
		}

		if (!hasUniform(rez, fragmentData.c_str(), "iFrameRate", "float"))
		{
			newFragmentStart += "\nuniform float iFrameRate;\n";
		}

		if (!hasUniform(rez, fragmentData.c_str(), "iFrame", "int"))
		{
			newFragmentStart += "\nuniform int iFrame;\n";
		}

		if (!hasUniform(rez, fragmentData.c_str(), "iMouse", "vec4"))
		{newFragmentStart += "\nuniform vec4 iMouse;\n";}

		if (!hasUniform(rez, fragmentData.c_str(), "iChannel0", "sampler2D"))
		{newFragmentStart += "\nuniform sampler2D iChannel0;\n";}

		if (!hasUniform(rez, fragmentData.c_str(), "iChannel1", "sampler2D"))
		{newFragmentStart += "\nuniform sampler2D iChannel1;\n";}

		if (!hasUniform(rez, fragmentData.c_str(), "iChannel2", "sampler2D"))
		{newFragmentStart += "\nuniform sampler2D iChannel2;\n";}

		if (!hasUniform(rez, fragmentData.c_str(), "iChannel3", "sampler2D"))
		{newFragmentStart += "\nuniform sampler2D iChannel3;\n";}

	#pragma endregion


		//add main function
		if (!hasMainFunction(rez, fragmentData.c_str()))
		{
			std::cout << "No main function!";

			newFragment +=
				R"(

				void main()
				{
				vec4 outColorLocalImpl54421234_5_ = vec4(0, 0, 0, 0);
				mainImage(outColorLocalImpl54421234_5_, gl_FragCoord.xy);
				)";
				
			newFragment += mainColorOutputName;

			newFragment += R"( = outColorLocalImpl54421234_5_;})";


		}
		else
		{
			std::cout << "Has main function!";
		}


		if (versionPos)
		{
			newFragment =
				std::string(newFragment.data(), newFragment.data() + versionPos) 
					+
					newFragmentStart
					+
				std::string(newFragment.data() + versionPos);
		}
		else
		{
			newFragment = newFragmentStart + newFragment;
		}



		shader.loadShaderProgramFromData(
			vertexData.c_str(), newFragment.c_str()
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

			if (strstr(name, uniformNames[j]))
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

	specialUniforms.iChannel0 = glGetUniformLocation(shader.id, "iChannel0");
	specialUniforms.iChannel1 = glGetUniformLocation(shader.id, "iChannel1");
	specialUniforms.iChannel2 = glGetUniformLocation(shader.id, "iChannel2");
	specialUniforms.iChannel3 = glGetUniformLocation(shader.id, "iChannel3");
	
	

	return true;
}

void RunningShader::bindAndSendUniforms(Renderer2D &renderer)
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
	glUniform1f(specialUniforms.iTime, accumulatedTime);
	glUniform1f(specialUniforms.iTimeDelta, deltaTime);
	glUniform1f(specialUniforms.iFrameRate, (float)currentFrameRate);
	glUniform1i(specialUniforms.iFrame, frameNumber);

	glm::vec4 mouseInput = {lastDownMousePos, lastClickMousePos};

	if (!mouseDown || !focused) { mouseInput.z *= -1; }
	if (!mouseClicked || !focused) { mouseInput.w *= -1; }

	glUniform4f(specialUniforms.iMouse, mouseInput.x, mouseInput.y, mouseInput.z, mouseInput.w); //todo


	glUniform1i(specialUniforms.iChannel0, 0);
	glUniform1i(specialUniforms.iChannel1, 1);
	glUniform1i(specialUniforms.iChannel2, 2);
	glUniform1i(specialUniforms.iChannel3, 3);

	
	for (int i = 0; i < 4; i++)
	{
		auto b = &inputBuffers[i];

		if (b->t.id == 0)
		{
			b->t.id = renderer.blackTexture.id;
		}

		b->t.bind(i);
	}


}

void RunningShader::updateSimulation(float deltaTime)
{
	this->deltaTime = deltaTime;
	accumulatedTime += deltaTime;

	frameNumber++;

	countedSeccond += deltaTime;
	countedFrameRate++;

	if (countedSeccond > 1.f)
	{
		countedSeccond -= 1;
		currentFrameRate = countedFrameRate;
		countedFrameRate = 0;
	}

	mouseDown = platform::isLMouseHeld() || platform::isRMouseHeld();
	mouseClicked = platform::isLMousePressed() || platform::isRMousePressed();
	
	currentMousePos = glm::clamp(currentMousePos, glm::vec2(0, 0), glm::vec2(w, h));

	if (focused)
	{
		if (mouseDown)
		{
			lastDownMousePos = currentMousePos;
		}

		if (mouseClicked)
		{
			lastClickMousePos = currentMousePos;
		}
	};

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


	webCamera.nrCameras = setupESCAPI();
	std::cout << "ESCAPI: " << webCamera.nrCameras << "\n";

}

void Renderer2D::render()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer2D::loadDefaultTextures()
{

	const unsigned char buff[] =
	{
		0x00,
		0x00,
		0x00,
		0xff
	};

	blackTexture.create1PxSquare((const char*)buff);

	for (auto &f : std::filesystem::directory_iterator(RESOURCES_PATH "defaultTextures"))
	{


		if (f.is_regular_file())
		{


			auto path = f.path();
			auto extension = path.extension().string();

			for (auto &i : extension) { i = tolower(i); }

			if (extension == ".png" ||
				extension == ".jpg" ||
				extension == ".jpeg" ||
				extension == ".bmp" ||
				extension == ".gif" ||
				extension == ".psd" ||
				extension == ".pic" ||
				extension == ".pnm" ||
				extension == ".hdr" ||
				extension == ".tga"
				)
			{

				gl2d::Texture t;
				t.loadFromFile(path.string().c_str());

				if (t.id)
				{
					Texture rezult;
					rezult.t = t;
					rezult.name = path.filename().string();

					defaultTextures.push_back(rezult);
				}
				

			}



		}



	}



}

void Renderer2D::WebCamera::startCapture()
{
	if (isCapturing) { return; }

	buffer.resize(w * h * 4);

	if (!t.id)
	{
		t.create1PxSquare();
	}

	SimpleCapParams params = {};
	params.mTargetBuf = (int*)buffer.data();
	params.mWidth = w;
	params.mHeight = h;

	if (initCapture(0, &params)) 
	{

		isCapturing = true;

		//std::cout << "Started! CAPTURE!";
	
	}
	else
	{
		isCapturing = false;
		//std::cout << "Coultnd start :(((";
	}


}

void Renderer2D::WebCamera::doCapture()
{
	if (!isCapturing)return;

	int hardCounter = 0;
	::doCapture(0); while (isCaptureDone(0) == 0) {};

	copyBuffer.resize(buffer.size());

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			copyBuffer[(x + y * w) * 4 + 0] = buffer[(x + (h - y - 1) * w) * 4 + 0];
			copyBuffer[(x + y * w) * 4 + 1] = buffer[(x + (h - y - 1) * w) * 4 + 1];
			copyBuffer[(x + y * w) * 4 + 2] = buffer[(x + (h - y - 1) * w) * 4 + 2];
			copyBuffer[(x + y * w) * 4 + 3] = 255;
		}
	}

	t.bind(5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, copyBuffer.data());
	glGenerateMipmap(GL_TEXTURE_2D);

}

void Renderer2D::WebCamera::deinitCapture()
{
	if (isCapturing)
	{
		::deinitCapture(0);
		isCapturing = 0;
	}

}
