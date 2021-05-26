#ifndef CUBE_H_
#define CUBE_H_

#include <memory>

#include "../shader.h"
#include "drawable.h"

class Cube : public Drawable
{
public:
    Cube(std::shared_ptr<Shader>& s);
    ~Cube()
    {
        glDeleteVertexArrays(1, &VAO);
	    glDeleteBuffers(1, &VBO);
	    glDeleteBuffers(1, &EBO);
    }
    void draw();
    
    unsigned int VBO, VAO, EBO;
};

#endif // CUBE_H_