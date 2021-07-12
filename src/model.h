#ifndef MODEL_H_
#define MODEL_H_

#include <iostream>
#include <vector>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"
#include "sampler.h"

class Model
{
public:
    Model(const char* path, std::shared_ptr<Shader>& sd, bool tex) : shader(sd), hasTex(tex)
    { 
        loadModel(path);
    }

    Model(std::string ind, std::string pos, std::string norm, std::string coeff, std::shared_ptr<Shader>& sd);

    void draw();
    void precomputeTransportCoefs(Sampler& sampler, glm::mat4 &model, bool recom);
    void setupMeshes();
    bool visiable(unsigned int ind, glm::vec3 vertex, glm::vec3 direction, glm::mat4 &model);


// private:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    std::shared_ptr<Shader> shader;
    bool hasTex;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    std::vector<char> readBinFile(std::string filename);

};

#endif // MODEL_H_