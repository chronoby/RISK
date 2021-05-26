#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include <memory>
#include <glm/gtc/type_ptr.hpp>

#include "../shader.h"

class Drawable
{
public:
    Drawable(){}
    Drawable(std::shared_ptr<Shader>& s) { shader = s; };
    virtual void draw() = 0;
    
    std::shared_ptr<Shader> shader;
    glm::vec3 translate;
    glm::vec3 rotate;
    glm::vec3 scale;
};

#endif // DRAWABLE_H_