#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>

#include "stb_image.h"

#include "shader.h"
#include "camera.h"
#include "gui.h"
#include "skybox.h"
#include "model.h"
#include "sampler.h"
#include "test.h"

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
Camera camera(glm::vec3(0.0f, 0.9f, 3.0f));

double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec4 lights[3] =
{
    glm::vec4(-2.0, 0.0, -2.0, 1.0),
    glm::vec4(2.0, 0.0, -2.0, 1.0),
    glm::vec4(0.0, 0.0, 2.0, 1.0)
};

glm::vec3 lightPos(0.0f, 0.8f, 0.4f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
float lightSize = 0.25f;
float lightAngle = 20.0f;

glm::vec3 manPos(0.0f, -0.3f, 0.0f);

bool showCursor = true;

glm::mat4 getInverseRotation(glm::mat4 model);

int main(int argc, char **argv)
{
    if(argc == 2 && strcmp(argv[1], "-test") == 0)
    {
        Test t;
        t.test();
        return 0;
    }
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    srand(time(nullptr));
    
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = mode->height;

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

	// glad load
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

    auto lightShader = std::make_shared<Shader>("shaders/light_vertex_shader.glsl", "shaders/light_fragment_shader.glsl");
	auto ashShader = std::make_shared<Shader>("shaders/ash_vertex_shader.glsl", "shaders/ash_fragment_shader.glsl");

    std::vector<glm::vec4> lightss =
    {
        glm::vec4(-2.0, 0.0, -2.0, 1.0),
        glm::vec4(2.0, 0.0, -2.0, 1.0),
        glm::vec4(0.0, 0.0, 2.0, 1.0)
    };
	int needreverse = 0;
	Sampler sampler(5);
	sampler.generateSamples(100);
	sampler.precomputeSHFunction();

    glm::mat4 modelModel = glm::mat4(1.0f);
	modelModel = glm::translate(modelModel, manPos);

    std::shared_ptr<Model> drawable = nullptr;
    bool recompute = false;
    if(argc == 1 || strcmp(argv[1], "-scene1") == 0)
    {
        if(argc == 3 && strcmp(argv[1], "-recompute"))
        {
            recompute = true;
        }

        drawable = std::make_shared<Model>("../assets/model/bunny/bunny.obj", ashShader, false);
        modelModel = glm::translate(modelModel, glm::vec3(0.0, 0.3, 0.0));
        modelModel = glm::scale(modelModel, glm::vec3(5.0f));
        drawable->precomputeTransportCoefs(sampler, modelModel, recompute);
    }
    else if(strcmp(argv[1], "-scene2") == 0)
    {
        drawable = std::make_shared<Model>("../assets/model/bin_model/sculpt0_ind", "../assets/model/bin_model/sculpt0_pos", "../assets/model/bin_model/sculpt0_norm", "../assets/model/bin_model/sc0_coeff", ashShader);
		modelModel = glm::translate(modelModel, glm::vec3(0.2, -0.3, 0.3));
		modelModel = glm::scale(modelModel, glm::vec3(0.05f));
		modelModel = glm::rotate(modelModel, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		modelModel = glm::rotate(modelModel, 3 * PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
		needreverse = 1;
    }
    drawable->setupMeshes();

	Gui gui;
    Skybox skybox;

	unsigned int lightVAO, lightVBO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glGenBuffers(1, &lightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lights), lights, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

    // bind framebuffer
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    // ash shader
    ashShader->use();
    glUniform3f(glGetUniformLocation(ashShader->ID, "dirLight.direction"), -1.0f, -3.0f, 1.0f);

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


		// render
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 lightModel = glm::mat4(1.0f);

		lightModel = glm::translate(lightModel, lightPos);
		lightModel = glm::scale(lightModel, glm::vec3(lightSize, lightSize, lightSize));
        lightModel = glm::rotate(lightModel, glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f));

		ashShader->use();

		glUniformMatrix4fv(glGetUniformLocation(ashShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(modelModel));
        glUniformMatrix4fv(glGetUniformLocation(ashShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(ashShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(ashShader->ID, "lightModel"), 1, GL_FALSE, glm::value_ptr(lightModel));
        
        glUniform1i(glGetUniformLocation(ashShader->ID, "lightVNum"), 3);
        glm::vec3 lightV[4];
        for(int i = 0; i < 3; ++i)
        {
			glm::vec4 tmp;
			if(needreverse)
				tmp = (getInverseRotation(model) * lightModel * lightss[i]);
			else
				tmp = lightModel * lightss[i];
			lightV[i] = glm::vec3(tmp) / tmp.w;
        }
        glUniform3fv(glGetUniformLocation(ashShader->ID, "lightV"), 3, (GLfloat*)(lightV));
		drawable->draw();

		// light
		lightShader->use();
		glUniformMatrix4fv(glGetUniformLocation(lightShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }    
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		lightPos.x += 0.01;
	}
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		lightPos.x -= 0.01;
	}
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		lightPos.y += 0.01;
	}
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		lightPos.y -= 0.01;
	}
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		lightPos.z += 0.01;
	}
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		lightPos.z -= 0.01;
	}
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		lightSize -= 0.001;
	}
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		lightSize += 0.001;
	}
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		lightAngle += 1.0;
	}
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
	double yoffset = lastY - ypos;
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


glm::mat4 getInverseRotation(glm::mat4 modelMatrix)
{
    glm::vec4 col = glm::column(modelMatrix, 0);
    
    float sf = 1.0 / sqrt(col[0]*col[0] + col[1]*col[1] + col[2]*col[2] + col[3]*col[3]);
    return glm::inverse(glm::scale(modelMatrix, glm::vec3(sf, sf, sf)));
}