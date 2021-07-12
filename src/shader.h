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
};

#endif // SHADER_H_
