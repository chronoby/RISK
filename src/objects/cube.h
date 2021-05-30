#ifndef CUBE_H_
#define CUBE_H_

#include <memory>

#include "../shader.h"
#include "drawable.h"

class Cube : public Drawable
{
public:
    Cube(std::shared_ptr<Shader>& sd, glm::vec3 t = glm::vec3(0.0, 0.0, 0.0), glm::vec3 r = glm::vec3(0.0, 0.0, 0.0),
        glm::vec3 s = glm::vec3(1.0, 1.0, 1.0));
    ~Cube()
    {
        glDeleteVertexArrays(1, &VAO);
	    glDeleteBuffers(1, &VBO);
	    glDeleteBuffers(1, &EBO);
    }
    virtual void draw() override;
    
    unsigned int VBO, VAO, EBO;
    std::shared_ptr<Shader> shader;
};

#endif // CUBE_H_