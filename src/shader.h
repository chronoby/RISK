#ifndef SHADER_H_
#define SHADER_H_

#include <glad/glad.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragPath);

    void use();
    void initCubeShader(unsigned int cubeTexture, unsigned int specularTexture, glm::vec3 lightColor);
    void setCubeShader(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos, 
        glm::vec3 cameraPos, int repeat, unsigned int depthMap);
};

#endif // SHADER_H_
