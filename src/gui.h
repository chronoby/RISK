#ifndef GUI_H_
#define GUI_H_

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class Gui
{
public:
	void init(GLFWwindow* win);
	void draw();

    bool skybox = false;
	bool room = false;
private:
	
	
};

#endif