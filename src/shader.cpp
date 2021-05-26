#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "Error: shader file read failed!" << std::endl;
    }
    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    // vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Error: vertex shader compile failed\n" << infoLog << std::endl;
	}

	// fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "Error: fragment shader compile failed\n" << infoLog << std::endl;
	}

	// link shaders
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "Error: shader program failed\n" << infoLog << std::endl;
    }

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use() 
{ 
    glUseProgram(ID);
}

void Shader::initCubeShader(unsigned int cubeTexture, unsigned int specularTexture, glm::vec3 lightColor)
{
    this->use();
    glUniform3f(glGetUniformLocation(ID, "dirLight.direction"), -1.0f, -3.0f, 1.0f);
    glUniform3f(glGetUniformLocation(ID, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(ID, "dirLight.diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(ID, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

    glUniform3fv(glGetUniformLocation(ID, "pointLight.color"), 1, glm::value_ptr(lightColor));
    
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.8f); // decrease the influence
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.05f); // low influence
    glUniform3fv(glGetUniformLocation(ID, "pointLight.ambient"), 1, glm::value_ptr(ambientColor));
    glUniform3fv(glGetUniformLocation(ID, "pointLight.diffuse"), 1, glm::value_ptr(diffuseColor));
    glUniform3f(glGetUniformLocation(ID, "pointLight.specular"), 1.0f, 1.0f, 1.0f);
    
    glUniform1i(glGetUniformLocation(ID, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(ID, "material.specular"), 1);
    glUniform1f(glGetUniformLocation(ID, "material.shininess"), 32.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularTexture);
}

void Shader::setCubeShader(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos, 
    glm::vec3 cameraPos, int repeat, unsigned int depthMap, glm::mat4 lightSpaceMatrix)
{
    this->use();
    glUniformMatrix4fv(glGetUniformLocation(ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(ID, "pointLight.position"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(ID, "viewPos"), 1, glm::value_ptr(cameraPos));
    glUniform1i(glGetUniformLocation(ID, "repeat"), repeat);
    glUniformMatrix4fv(glGetUniformLocation(ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    glUniform1i(glGetUniformLocation(ID, "shadowMap"), 2);
    glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}