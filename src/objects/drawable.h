#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include <memory>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"

class Drawable
{
public:
    Drawable(glm::vec3 t = glm::vec3(0.0, 0.0, 0.0), glm::vec3 r = glm::vec3(0.0, 0.0, 0.0), glm::vec3 s = glm::vec3(1.0, 1.0, 1.0)) :
        translate(t), rotate(r), scale(s) { }
    Drawable(std::shared_ptr<Shader>& s) { shader = s; };
    virtual void draw() = 0;
    
    std::shared_ptr<Shader> shader;
    glm::vec3 translate;
    glm::vec3 rotate;
    glm::vec3 scale;
};

#endif // DRAWABLE_H_