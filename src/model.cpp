#include <iostream>
#include <fstream>
#include "model.h"

#include "stb_image.h"

unsigned int TextureFromFile(const char *path, const std::string &directory);

void Model::draw()
{
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].draw(*shader, hasTex);
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "Error: ASSIMP import failed!\n" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        if(mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool loaded = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                loaded = true;
                break;
            }
        }
        if (!loaded)
        {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const std::string &directory)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Model::precomputeTransportCoefs(Sampler &sampler, glm::mat4 &model, bool recal)
{
    if(!recal)
    {
        double weight = 4.0 * PI / sampler.sampleNum;
        std::ifstream inf("../tranport_coeff/bunny.txt");
        for(auto i = 0; i < meshes.size(); ++i)
        {
            // std::cout << "mesh: " << i << " / " << meshes.size() << std::endl;
            Mesh &mesh = meshes[i];
            for (int j = 0; j < mesh.vertices.size(); ++j)
            {
                // std::cout << "vertices: " << j << " / " << mesh.vertices.size() << std::endl;
                Vertex &vertex = mesh.vertices[j];
                float transportCoef[25] = { 0 };
                for(int l = 0; l < 25; ++l)
                {
                    inf >> transportCoef[l];
                    // transportCoef[i] /= weight;
                }
                vertex.tcoe0 = glm::vec4(transportCoef[0], transportCoef[1], transportCoef[2], transportCoef[3]);
                vertex.tcoe1 = glm::vec4(transportCoef[4], transportCoef[5], transportCoef[6], transportCoef[7]);
                vertex.tcoe2 = glm::vec4(transportCoef[8], transportCoef[9], transportCoef[10], transportCoef[11]);
                vertex.tcoe3 = glm::vec4(transportCoef[12], transportCoef[13], transportCoef[14], transportCoef[15]);
                vertex.tcoe4 = glm::vec4(transportCoef[16], transportCoef[17], transportCoef[18], transportCoef[19]);
                vertex.tcoe5 = glm::vec4(transportCoef[20], transportCoef[21], transportCoef[22], transportCoef[23]);
                vertex.tcoe6 = transportCoef[24];
            }
        }
        return;
    }

    std::ofstream ouf;
    ouf.open("../tranport_coeff/newcoeff.txt");
    bool unshadowed = true;

    for(auto i = 0; i < meshes.size(); ++i)
    {
        // std::cout << "mesh: " << i << " / " << meshes.size() << std::endl;
        Mesh &mesh = meshes[i];
        for (int j = 0; j < mesh.vertices.size(); ++j)
        {
            float transportCoef[25] = { 0 };

            std::cout << "vertices: " << j << " / " << mesh.vertices.size() << std::endl;
            Vertex &vertex = mesh.vertices[j];
            glm::vec4 pos4 = model * glm::vec4(vertex.Position, 1.0f);
            glm::vec3 pos= glm::vec3(pos4.x / pos4.w, pos4.y / pos4.w, pos4.z / pos4.w);
            glm::vec4 normal4 = glm::transpose(glm::inverse(model)) * glm::vec4(vertex.Normal, 1.0f);
            glm::vec3 normal = glm::vec3(normal4.x, normal4.y, normal4.z);
            for (int k = 0; k < sampler.sampleNum; ++k)
            {
                Sample& sample = sampler.samples[k];
                if (unshadowed || visiable(j, vertex.Position, sample.cartesianCoord, model))
                {
                    // float G = std::max(0.0f, glm::dot(vertex.Normal, sample.cartesianCoord));
                    float G = std::max(0.0f, glm::dot(normal, sample.cartesianCoord));
                    // std::cout << sample.cartesianCoord.x << " "<< sample.cartesianCoord.y << " "<< sample.cartesianCoord.z << " " << std::endl;
                    for (int l = 0; l < sampler.shBands * sampler.shBands; ++l)
                    {
                        float sh_fun = sample.shFunctions[l];
                        transportCoef[l] += (sh_fun * G);
                    }
                }
            }
            double weight = 4.0 * PI / sampler.sampleNum;
            for (int l = 0; l < sampler.shBands * sampler.shBands; ++l)
            {
                transportCoef[l] *= weight;
                mesh.transport_coefs.push_back(transportCoef[l]);
                ouf << transportCoef[l] << " ";
                // std::cout << transportCoef[l] * weight << " ";
            }
            ouf << std::endl;
            vertex.tcoe0 = glm::vec4(transportCoef[0], transportCoef[1], transportCoef[2], transportCoef[3]);
            vertex.tcoe1 = glm::vec4(transportCoef[4], transportCoef[5], transportCoef[6], transportCoef[7]);
            vertex.tcoe2 = glm::vec4(transportCoef[8], transportCoef[9], transportCoef[10], transportCoef[11]);
            vertex.tcoe3 = glm::vec4(transportCoef[12], transportCoef[13], transportCoef[14], transportCoef[15]);
            vertex.tcoe4 = glm::vec4(transportCoef[16], transportCoef[17], transportCoef[18], transportCoef[19]);
            vertex.tcoe5 = glm::vec4(transportCoef[20], transportCoef[21], transportCoef[22], transportCoef[23]);
            vertex.tcoe6 = transportCoef[24];
        }
    }
}

void Model::setupMeshes()
{
    for(auto i = 0; i < meshes.size(); ++i)
    {
        meshes[i].setupMesh();
    }
}

bool Model::visiable(unsigned int ind, glm::vec3 vertex, glm::vec3 direction, glm::mat4 &model)
{
    bool visible = true;
    for (int i = 0; i < meshes.size(); ++i)
    {
        auto mesh = meshes[i];
        for (int j = 0; j < mesh.indices.size(); j += 3)
        {
            if ((ind != mesh.indices[j]) && (ind != mesh.indices[j + 1]) && (ind != mesh.indices[j + 2]))
            {
                glm::vec3 v0 = mesh.vertices[mesh.indices[j    ]].Position;
                // glm::vec4 pos40 = model * glm::vec4(v0, 1.0f);
                // glm::vec3 pos0= glm::vec3(pos40.x / pos40.w, pos40.y / pos40.w, pos40.z / pos40.w);

                glm::vec3 v1 = mesh.vertices[mesh.indices[j + 1]].Position;
                // glm::vec4 pos41 = model * glm::vec4(v1, 1.0f);
                // glm::vec3 pos1= glm::vec3(pos41.x / pos41.w, pos41.y / pos41.w, pos41.z / pos41.w);

                glm::vec3 v2 = mesh.vertices[mesh.indices[j + 2]].Position;
                // glm::vec4 pos42 = model * glm::vec4(v2, 1.0f);
                // glm::vec3 pos2= glm::vec3(pos42.x / pos42.w, pos42.y / pos42.w, pos42.z / pos42.w);

                // visible = !mesh.rayPassTriangle(vertex, direction, pos0, pos1, pos2);
                visible = !mesh.rayPassTriangle(vertex, direction, v0, v1, v2);
                if (!visible)
                    break;
            }
        }
    }
    return(visible);
}

Model::Model(std::string ind, std::string pos, std::string norm, std::string coeff, std::shared_ptr<Shader>& sd)
{
    hasTex = false;
    shader = sd;
    std::vector<char> indData = readBinFile(ind);
    std::vector<char> posData =  readBinFile(pos);
    std::vector<char> normData =  readBinFile(norm);
    std::vector<char> coeffData = readBinFile(coeff);

    int numVertices = posData.size() / sizeof(float) / 3;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    char *byteData = &(posData[0]);
    float *mPositions = reinterpret_cast<float *> (byteData);

    byteData = &(indData[0]);
    uint32_t *mIndices = reinterpret_cast<uint32_t *> (byteData);

    byteData = &(normData[0]);
    float *mNormals = reinterpret_cast<float *> (byteData);

    byteData = &(coeffData[0]);
    float *mCoes = reinterpret_cast<float *> (byteData);

    int index = 0;
    int coeind = 0;
    for (int i = 0; i < numVertices; i++) {
        Vertex vertex;
        glm::vec3 vector; // Use placeholder vector to read out assimp vector values
        // Positions
        vector.x = mPositions[index];
        vector.y = mPositions[index + 1];
        vector.z = mPositions[index + 2];
        vertex.Position = vector;

        // Normals
        vector.x = mNormals[index];
        vector.y = mNormals[index + 1];
        vector.z = mNormals[index + 2];
        vertex.Normal = vector;

        vertex.tcoe0 = glm::vec4(mCoes[coeind], mCoes[coeind + 1], mCoes[coeind + 2], mCoes[coeind + 3]);
        coeind += 4;
        vertex.tcoe1 = glm::vec4(mCoes[coeind], mCoes[coeind + 1], mCoes[coeind + 2], mCoes[coeind + 3]);
        coeind += 4;
        vertex.tcoe2 = glm::vec4(mCoes[coeind], mCoes[coeind + 1], mCoes[coeind + 2], mCoes[coeind + 3]);
        coeind += 4;
        vertex.tcoe3 = glm::vec4(mCoes[coeind], mCoes[coeind + 1], mCoes[coeind + 2], mCoes[coeind + 3]);
        coeind += 4;
        vertex.tcoe4 = glm::vec4(mCoes[coeind], mCoes[coeind + 1], mCoes[coeind + 2], mCoes[coeind + 3]);
        coeind += 4;
        vertex.tcoe5 = glm::vec4(mCoes[coeind], mCoes[coeind + 1], mCoes[coeind + 2], mCoes[coeind + 3]);
        coeind += 4;
        vertex.tcoe6 = mCoes[coeind++];

        vertices.push_back(vertex);
        index += 3;
    }

    int numIndices = indData.size() / sizeof(uint32_t);
        
    // Get indices for all faces
    for(unsigned int i = 0; i < numIndices; i++) {          
        indices.push_back(mIndices[i]);
    }
    meshes.push_back(Mesh(vertices, indices));
}

std::vector<char> Model::readBinFile(std::string filename)
{
    std::ifstream infile(filename, std::ifstream::binary);
    infile.seekg(0, infile.end);
    int length = infile.tellg();
    infile.seekg(0, infile.beg);

    std::vector<char> buffer(length);
    infile.read(&buffer[0], length);

    return buffer;
}