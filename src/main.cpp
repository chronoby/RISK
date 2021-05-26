#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"
#include "camera.h"
#include "gui.h"
#include "skybox.h"
#include "shadow/shadowmap.h"
#include "objects/model.h"
#include "objects/cube.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const *path);
unsigned int loadCubemap(std::vector<std::string> faces);

unsigned int SCR_WIDTH = 1600;
unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.5f, 3.0f));

double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(0.0f, 4.0f, -15.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

glm::vec3 manPos(1.0f, 2.0f, -6.0f);

bool showCursor = true;

int main()
{
	// glfw initialize and config
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = mode->height;

	// glfw window create
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Renderer", nullptr, nullptr);
	glfwMaximizeWindow(window);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwMaximizeWindow(window);
	glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// glad load opengl function pointer
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

    auto cubeShader = std::make_shared<Shader>("shaders/cube_vertex_shader.glsl", "shaders/cube_fragment_shader.glsl");
    auto lightShader = std::make_shared<Shader>("shaders/light_vertex_shader.glsl", "shaders/light_fragment_shader.glsl");
    auto depthShader = std::make_shared<Shader>("shaders/depth_vertex_shader.glsl", "shaders/depth_fragment_shader.glsl");
	
    Shader skyboxShader("shaders/skybox_vertex_shader.glsl", "shaders/skybox_fragment_shader.glsl");

	Model man("../assets/model/nanosuit/nanosuit.obj", cubeShader);
	Cube cube1(cubeShader);
	Cube cube2(cubeShader);
	Cube cube3(cubeShader);

	Gui gui;
    Skybox skybox(skyboxShader);
    ShadowMap shadowMap;

	glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, -3.0f),
        glm::vec3( 1.0f, -1.0f, -5.0f)
    };

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cube1.VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture
	unsigned int cubeTexture = loadTexture("../assets/image/container2.png");
	unsigned int specularTexture = loadTexture("../assets/image/container2_specular.png");

    // framebuffer
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // 将它附加到当前绑定的帧缓冲对象
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	gui.init(window);
	
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		// input
		processInput(window);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// shadow pass
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, shadowMap.SHADOW_WIDTH, shadowMap.SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		float near_plane = 1.0f, far_plane = 17.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(glm::vec3(3.0f, 9.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		depthShader->use();
		glUniformMatrix4fv(glGetUniformLocation(depthShader->ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		for (unsigned int i = 0; i < 3; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(glGetUniformLocation(depthShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glBindVertexArray(cube1.VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
        }
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// render pass
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		
		// draw triangle
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 lightmodel = glm::mat4(1.0f);
        lightmodel = glm::scale(lightmodel, glm::vec3(0.2f));
		lightmodel = glm::rotate(lightmodel, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		lightmodel = glm::translate(lightmodel, lightPos);
		glm::vec3 lightPosition= lightmodel * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        
		for (unsigned int i = 0; i < 3; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			int repeat = 1;
            cubeShader->initCubeShader(cubeTexture, specularTexture, lightColor);

			cubeShader->setCubeShader(model, view, projection, lightPosition, camera.Position, repeat, shadowMap.depthMap, lightSpaceMatrix);
            cube1.draw();
        }
        if(gui.room)
        {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
			model = glm::scale(model, glm::vec3(15.0f));
			glUniform1i(glGetUniformLocation(cubeShader->ID, "repeat"), 5);
			glUniformMatrix4fv(glGetUniformLocation(cubeShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
		glUniform1i(glGetUniformLocation(cubeShader->ID, "repeat"), 1);
		glm::mat4 ManModel = glm::mat4(1.0f);
		ManModel = glm::scale(ManModel, glm::vec3(0.1f));
		ManModel = glm::translate(ManModel, manPos);
		glUniformMatrix4fv(glGetUniformLocation(cubeShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(ManModel));
		man.draw(*cubeShader);

		// light
		lightShader->use();
		glUniformMatrix4fv(glGetUniformLocation(lightShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(lightmodel));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// skybox
        if(gui.skybox)
        {
            view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            skybox.draw(view, projection);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        gui.draw(texColorBuffer, width, height);
		

        glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		if (showCursor)
		{
			showCursor = !showCursor;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			showCursor = !showCursor;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (showCursor)
		return;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!showCursor)
		return;
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const *path)
{
	unsigned int texture;
	glGenTextures(1, &texture);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
		else {
			format = GL_RGB;
		}
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	
	return texture;
}
