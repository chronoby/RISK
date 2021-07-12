#ifndef SKYBOX_H_
#define SKYBOX_H_

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class Skybox
{
public:
    Skybox();
    void draw(glm::mat4 view, glm::mat4 projection);
    unsigned int loadCubemap(std::vector<std::string> faces);

    unsigned int skyboxVAO, skyboxVBO;
    unsigned int cubemapTexture;
    Shader shader;
};

#endif