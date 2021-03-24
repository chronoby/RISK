#ifndef SHADER_H_
#define SHADER_H_

#include <glad/glad.h>

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
};

#endif // SHADER_H_
