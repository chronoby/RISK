#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <cstdlib>
#include "mesh.h"
#include "sampler.h"

Mesh::Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind, std::vector<Texture> text)
{
    this->vertices = vert;
    this->indices = ind;
    this->textures = text;

    // setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind)
{
    this->vertices = vert;
    this->indices = ind;

    // setupMesh();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);   
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);   
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);   
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tcoe0));

    glEnableVertexAttribArray(4);   
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tcoe1));
    
    glEnableVertexAttribArray(5);   
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tcoe2));
    
    glEnableVertexAttribArray(6);   
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tcoe3));
    
    glEnableVertexAttribArray(7);   
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tcoe4));
    
    glEnableVertexAttribArray(8);   
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tcoe5));
    
    glEnableVertexAttribArray(9);   
    glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tcoe6));

    glBindVertexArray(0);
}

void Mesh::draw(Shader shader, bool hasTex) 
{
    if (hasTex)
    {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);

            glUniform1f(glGetUniformLocation(shader.ID, ("material." + name + number).c_str()), i);

            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

bool Mesh::rayPassTriangle(glm::vec3& p, glm::vec3& d, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2)
{
    glm::vec3 e1 = glm::vec3(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
    glm::vec3 e2 = glm::vec3(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
    glm::vec3 h;
    h = glm::cross(d, e2);
    float a = glm::dot(e1, h);
    if (a > -0.00001f && a < 0.00001f)
        return false;
    float f = 1.0f / a;
    glm::vec3 s = glm::vec3(p.x - v0.x, p.y - v0.y, p.z - v0.z);
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f)
        return false;
    glm::vec3 q;
    q = glm::cross(s, e1);
    float v = f * dot(d, q);
    if (v < 0.0f || u + v > 1.0f)
        return false;
    float t = glm::dot(e2, q) * f;
    if (t < 0.0f)
        return false;
    return(true);
}

