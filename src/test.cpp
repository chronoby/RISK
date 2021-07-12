#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shFunction.h"
#include "test.h"
#include "model.h"
#include "sampler.h"

float calSolidAngle(glm::vec3 omega[5], int lightVNum)
{
    float res = 0.0f;
    for (int i = 0; i < lightVNum; ++i)
    {
        int l = (i + lightVNum - 1) % lightVNum;
        int r = (i + 1) % lightVNum;
        glm::vec3 cross1 = cross(omega[i], omega[l]);
        glm::vec3 cross2 = cross(omega[i], omega[r]);

        res += acos(dot(cross1, cross2) / (length(cross1) * length(cross2)));
    }
    return res - (lightVNum - 2) * PI;
}

float P(int n, float x)
{
    if (n == 0)
        return 1;
    if (n == 1)
        return x;
    if (n == 2)
        return (0.5 * (3.0 * x * x - 1));
    if (n == 3)
        return (0.5 * (5.0 * pow(x, 3.0) - 3.0 * x));
    if (n == 4)
        return (1 / 8 * (35 * pow(x, 4.0) - 30 * x * x + 3));
}
void Test::test()
{
    std::cout << "light vertex position: (-2.0, 1.0, -2.0), (2.0, 1.0, -2.0), (0.0, 1.0, 2.0)" << std::endl;
    std::cout << "shading point position: (0.0, 0.0, 0.0)" << std::endl;

    int lightVNum = 3;
    glm::vec3 lightV[3] =
    {
        glm::vec3(-2.0, 1.0, -2.0),
        glm::vec3(2.0, 1.0, -2.0),
        glm::vec3(0.0, 1.0, 2.0)
        // glm::vec3(4.0, 1.0, 2.0)
    };
    glm::vec3 aPos = glm::vec3(0.0, 0.0, 0.0);
    int shOrder = 4;
    glm::vec3 lobeDir[9] =
    {
        glm::vec3(0.866025, -0.500001, -0.000004),
        glm::vec3(-0.759553, 0.438522, -0.480394),
        glm::vec3(-0.000002, 0.638694, 0.769461),
        glm::vec3(-0.000004, -1.000000, -0.000004),
        glm::vec3(-0.000007, 0.000003, -1.000000),
        glm::vec3(-0.000002, -0.638694, 0.769461),
        glm::vec3(-0.974097, 0.000007, -0.226131),
        glm::vec3(-0.000003, 0.907079, -0.420960),
        glm::vec3(-0.960778, 0.000007, -0.277320)
    };

    glm::vec3 lightVPos[5];
    glm::vec3 omega[5];
    glm::vec3 lambda[5];
    glm::vec3 miu[5];

    float gamma[5];
    float S[5];
    float L[9][5];
    float a[5];
    float b[5];
    float c[5];
    float Bb[5];

    float B[5][5];
    float C[5][5];
    float D[5][5];

    float coe[25];

    for (int i = 0; i < lightVNum; ++i)
    {

        lightVPos[i] = lightV[i] - aPos;
        omega[i] = normalize(lightVPos[i]);

    }
    for (int i = 0; i < lightVNum; ++i)
    {
        int j = (i + 1) % lightVNum;
        lambda[i] = cross(normalize(cross(omega[i], omega[j])), omega[i]);
        miu[i] = cross(omega[i], lambda[i]);
        gamma[i] = acos(dot(omega[i], omega[j]));

    }
    S[0] = calSolidAngle(omega, lightVNum);

    for (int i = 0; i < 2 * shOrder + 1; ++i)
    {
        S[1] = 0;
        for (int j = 0; j < lightVNum; ++j)
        {
            a[j] = dot(lobeDir[i], omega[j]);
            
            b[j] = dot(lobeDir[i], lambda[j]);
            c[j] = dot(lobeDir[i], miu[j]);
            S[1] += 0.5f * c[j] * gamma[j];
            B[0][j] = gamma[j];
            B[1][j] = a[j] * sin(gamma[j]) - b[j] * cos(gamma[j]) + b[j];
            D[0][j] = 0;
            D[1][j] = gamma[j];
            D[2][j] = 3 * B[1][j];
            
        }
        L[i][1] = sqrt(3 / (4 * PI)) * S[1];
        for (int j = 0; j < 2; ++j)
        {
            Bb[j] = 0;
            for (int k = 0; k < lightVNum; ++k)
            {
                Bb[j] = Bb[j] + c[k] * B[j][k];
            }
        }

        for (int j = 2; j < 5; ++j)
        {
            float jj = float(j);
            Bb[j] = 0;
            for (int k = 0; k < lightVNum; ++k)
            {
                C[j - 1][k] = 1.0 / jj * ((a[k] * sin(gamma[k]) - b[k] * cos(gamma[k])) * P(j - 1, a[k] * cos(gamma[k]) + b[k] * sin(gamma[k])) + b[k] * P(j - 1, a[k]) + (a[k] * a[k] + b[k] * b[k] - 1) * D[j - 1][k] + (j - 1) * B[j - 2][k]);
                B[j][k] = (2 * jj - 1) / jj * C[j - 1][k] - (jj - 1) / jj * B[j - 2][k];
                
                Bb[j] = Bb[j] + c[k] * B[j][k];
                D[j][k] = (2 * jj - 1) * B[j - 1][k] + D[j - 2][k];;

            }
            
            S[j] = ((2.0 * jj - 1) * Bb[j - 1] + ((jj - 2) * (jj - 1)) * S[j - 2]) / (jj * (jj + 1));
            L[i][j] = S[j];
        }
    }


    coe[0] = 0.282095 * S[0];

    coe[1] = 2.1995339 * L[0][1] + 2.50785367 * L[1][1] + 1.56572711 * L[2][1];
    coe[2] = -1.82572523 * L[0][1] + (-2.08165037) * L[1][1];
    coe[3] = 2.42459869 * L[0][1] + 1.44790525 * L[1][1] + 0.90397552 * L[2][1];

    coe[4] = -1.33331385 * L[0][2] + (-0.66666684) * L[3][2] + (-0.99999606) * L[4][2];
    coe[5] = 1.1747938 * L[2][2] + (-0.47923799) * L[3][2] + (-0.69556433) * L[4][2];
    coe[6] = L[4][2];
    coe[7] = -1.21710396 * L[0][2] + 1.58226094 * L[1][2] + 0.67825711 * L[2][2] + (-0.27666329) * L[3][2] + (-0.76671491) * L[4][2];
    coe[8] = -1.15470843 * L[3][2] + (-0.57735948) * L[4][2];

    coe[9] = -0.418128476395 * L[2][3] + 1.04704832111 * L[3][3] + 0.418135743058 * L[5][3];
    coe[10] = -0.217803921828 * L[0][3] + 1.61365275071 * L[1][3] + (-0.0430709310435) * L[2][3] + (-1.08141635635) * L[3][3] + 0.730013109257 * L[4][3] + (-0.906789272616) * L[5][3];
    coe[11] = 0.539792926181 * L[2][3] + 0.281276817357 * L[3][3] + (-0.53979650602) * L[5][3];
    coe[12] = -1.0 * L[4][3];
    coe[13] = -1.88563738164 * L[0][3] + 0.934959388519 * L[2][3] + (-1.39846078802) * L[3][3] + (-0.934977410564) * L[5][3];
    coe[14] = -0.822588107798 * L[2][3] + 0.0250955547337 * L[4][3] + (-0.822583092847) * L[5][3];
    coe[15] = -1.14577301943 * L[0][3] + 1.03584677217 * L[2][3] + (-0.849735800355) * L[3][3] + (-0.438905584229) * L[4][3] + (-0.100364975081) * L[5][3] + (-1.36852983602) * L[6][3];

    coe[16] = -0.694140591095 * L[0][4] + (-1.46594132085) * L[1][4] + (-3.76291455607) * L[2][4] + (-4.19771773174) * L[3][4] + (-4.41452625915) * L[4][4] + (-5.21937739623)
        * L[5][4] + 30.1096083902 * L[6][4] + (-0.582891410482) * L[7][4] + (-25.58700736) * L[8][4];
    coe[17] = -0.776237001754 * L[2][4] + (-0.497694700099) * L[3][4] + 0.155804529921 * L[4][4] + 0.255292423057 * L[5][4] + (-0.00123151211175) * L[6][4] + 0.86352262597 * L[7][4] + 0.00106337156796 * L[8][4];
    coe[18] = 1.14732747049 * L[0][4] + (-1.93927453351) * L[1][4] + (-4.97819284362) * L[2][4] + (-4.52057526927) * L[3][4] + (-7.00211058681) * L[4][4] + (-6.90497275343)
        * L[5][4] + 39.8336896922 * L[6][4] + (-0.771083185249) * L[7][4] + (-33.8504871326) * L[8][4];
    coe[19] = 0.392392485498 * L[2][4] + (-0.469375435363) * L[3][4] + 0.146862690526 * L[4][4] + (-0.883760925422) * L[5][4] + 0.81431736181 * L[7][4];
    coe[20] = 1.00015572278 * L[4][4] + (-0.00110374505123) * L[6][4] + 0.000937958411459 * L[8][4];
    coe[21] = 7.51111593422 * L[2][4] + 6.56318513992 * L[3][4] + 7.31626822687 * L[4][4] + 7.51109857163 * L[5][4] + (-51.4260730066) * L[6][4] + 43.7016908482 * L[8][4];
    coe[22] = -0.61727564343 * L[2][4] + 0.205352092062 * L[3][4] + (-0.461764665742) * L[4][4] + (-0.617286413191) * L[5][4];
    coe[23] = 6.71336600734 * L[2][4] + 5.24419547627 * L[3][4] + 7.13550000457 * L[4][4] + 6.71337558899 * L[5][4] + (-51.8339912003) * L[6][4] + 45.9921960339 * L[8][4];
    coe[24] = 0.466450172383 * L[2][4] + 1.19684418958 * L[3][4] + (-0.158210638771) * L[4][4] + 0.466416144347 * L[5][4] + 0.000906975300098 * L[6][4];

    std::cout << "SH coeff of lighting function" << std::endl;
    std::cout << "Analytic method:" << std::endl;

    for (int i = 0; i < 25; ++i)
    {
        std::cout << coe[i] << " ";
    }
    std::cout << std::endl;

    // Sampler sampler(5);
	// sampler.generateSamples(1000);
	// sampler.precomputeSHFunction();
    // float lightingCoef[25] = { 0 };

    // for (int i = 0; i < sampler.sampleNum; ++i)
    // {
    //     Sample& sample = sampler.samples[i];
    //     Mesh m;
    //     if(m.rayPassTriangle(aPos, sample.cartesianCoord, lightV[0], lightV[1], lightV[2]))
    //     {
    //         for (int l = 0; l < sampler.shBands * sampler.shBands; ++l)
    //         {
    //             float sh_fun = sample.shFunctions[l];
    //             lightingCoef[l] += (sh_fun);
    //         }
    //     }
        
    // }
    // double weight = 4.0 * PI / sampler.sampleNum;
    // std::cout << "Monte carlo integration method:" << std::endl;
    // for (int l = 0; l < sampler.shBands * sampler.shBands; ++l)
    // {
    //     lightingCoef[l] *= weight;
    //     std::cout << lightingCoef[l] << " ";
    // }
}