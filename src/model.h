#ifndef MODEL_H_
#define MODEL_H_

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

class Model
{
public:
    Model(char *path) { loadModel(path); }
    void Draw(Shader shader);

private:
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
}

#endif // MODEL_H_