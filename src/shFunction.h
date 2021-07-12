#ifndef SHFUNCTION_H_
#define SHFUNCTION_H_

constexpr float PI = 3.14159265358979323846;

class SHFunction
{
public:
    float sh(float theta, float phi, int m, int l);

    float legendre(float x, int m, int l);
    float fac(int n);
    float doubleFac(int n);
    float K(int m, int l);
    float EvalSHSlow(int l, int m, float phi, float theta);

};

#endif // SHFUNCTION_H_
