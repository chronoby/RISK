#ifndef SHADER_H_
#define SHADER_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    unsigned int ID;
    Shader() {}
    Shader(const char* vertexPath, const char* fragPath);
    Shader(const Shader& s)
    {
        ID = s.ID;
    }

    void use();
    void initCubeShader(unsigned int cubeTexture, unsigned int specularTexture, glm::vec3 lightColor);
    void setCubeShader(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos, 
        glm::vec3 cameraPos, int repeat, unsigned int depthMap, glm::mat4 lightSpaceMatrix);
};

#endif // SHADER_H_
