#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "gui.h"

void Gui::init(GLFWwindow* win)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	const char* glsl_version = "#version 130";
	ImGui_ImplGlfw_InitForOpenGL(win, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void Gui::draw(unsigned int tex, int width, int height)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
    float topWindowWidth = static_cast<float>(width);
    float topWindowHeight = static_cast<float>(height);

	bool show_demo_window = true;
    ImVec2 pos, size;
    ImGui::SetNextWindowPos(ImVec2(0.0, 0.0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300.0, topWindowHeight), ImGuiCond_Once);

	ImGui::Begin("Properties");

    ImGui::Text("properties");
	ImGui::Checkbox("skybox", &skybox);
	ImGui::Checkbox("room", &room);
	ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(300.0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(size.x - 300.0, topWindowHeight), ImGuiCond_Once);
    ImGui::Begin( "##Child2" );
    ImGui::Text("scene");
    ImVec2 wsize = ImVec2(1600, 900);
	ImGui::Image((ImTextureID)tex, wsize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

	ImGui::Render();
	
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
