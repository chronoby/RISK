#ifndef MODEL_H_
#define MODEL_H_

#include <iostream>
#include <vector>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "../shader.h"
#include "drawable.h"

class Model : public Drawable
{
public:
    Model(const char* path, std::shared_ptr<Shader>& sd, glm::vec3 t = glm::vec3(0.0, 0.0, 0.0), 
        glm::vec3 r = glm::vec3(0.0, 0.0, 0.0), glm::vec3 s = glm::vec3(1.0, 1.0, 1.0)) :
        shader(sd), Drawable(t, r, s)
    { 
        loadModel(path);
    }
    virtual void draw() override;

private:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    std::shared_ptr<Shader> shader;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif // MODEL_H_