#ifndef SAMPLER_H_
#define SAMPLER_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shFunction.h"

class Spherical
{
public:
    Spherical(float t, float p) : theta(t), phi(p) { }

    float theta;
    float phi;
};

class Sample
{
public:
    Sample(float t, float p, glm::vec3 cartesian) : sphericalCoord(t, p), cartesianCoord(cartesian) { }
    Spherical sphericalCoord;
    glm::vec3 cartesianCoord;
    std::vector<float> shFunctions;
};

class Sampler
{
public:
    Sampler(int band) : shBands(band) { }
    void generateSamples(int n);
    void precomputeSHFunction();
    // void calWeight();
    // bool projectTransport(Model& model);


// private:
    float random();
    std::vector<Sample> samples;
    int sampleNum;
    int shBands;
    std::vector<glm::vec3> coefs;
};

#endif // SAMPLER_H_
