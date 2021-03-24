#ifndef MESH_H_
#define MESH_H_
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoords;
};

struct Texture
{
    unsigned int id;
    string type;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind, std::vector<Texture> text);
    void Draw(Shader shader);
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};


#endif // MESH_H_