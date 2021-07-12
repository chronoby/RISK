#include <cmath>
#include <iostream>
#include "sampler.h"

void Sampler::generateSamples(int n)
{
    for(int i = 0; i < n; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            float a = (static_cast<float>(i) + random()) / static_cast<float>(n);
            float b = (static_cast<float>(j) + random()) / static_cast<float>(n);
            
            float theta = 2 * acos(sqrt(1 - a));
            float phi = 2 * PI * b;
            // std::cout << theta << " " << phi << std::endl;
            float x = sin(theta) * cos(phi);
            float y = sin(theta) * sin(phi);
            float z = cos(theta);
            samples.push_back(Sample(theta, phi, glm::vec3(x, y, z)));
        }
    }
    sampleNum = n * n;
}

void Sampler::precomputeSHFunction()
{
    SHFunction shf;
    for(int i = 0; i < sampleNum; ++i)
    {
        float theta = samples[i].sphericalCoord.theta;
        float phi = samples[i].sphericalCoord.phi;
        for(int l = 0; l < shBands; ++l)
        {
            for(int m = -l; m <= l; ++m)
            {
                samples[i].shFunctions.push_back(shf.sh(theta, phi, m, l));
            }
        }
    }
}

float Sampler::random()
{
    return static_cast<float>((rand() % 1000)) / 1000.0f;
}
