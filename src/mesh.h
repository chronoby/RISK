#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
    glm::vec4 tcoe0;
    glm::vec4 tcoe1;
    glm::vec4 tcoe2;
    glm::vec4 tcoe3;
    glm::vec4 tcoe4;
    glm::vec4 tcoe5;
    float tcoe6;
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    Mesh() { }
    Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind, std::vector<Texture> text);
    Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind);
    void draw(Shader shader, bool hasTex);
    // void precomputeTransportCoefs(Sampler& sampler);
    bool rayPassTriangle(glm::vec3& p, glm::vec3& d, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2);
    // bool visiable(Model& model, glm::vec3 vertex, glm::vec3 direction);
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    std::vector<float> transport_coefs;
// private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};


#endif // MESH_H_