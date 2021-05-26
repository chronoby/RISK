#ifndef GUI_H_
#define GUI_H_

class Gui
{
public:
	void init(GLFWwindow* win);
	void draw(unsigned int tex, int width, int height);

    bool skybox = false;
	bool room = false;
private:
};

#endif