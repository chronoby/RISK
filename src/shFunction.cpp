#include <cmath>
#include "shFunction.h"

float SHFunction::EvalSHSlow(int l, int m, float phi, float theta) {
  float kml = sqrt((2.0 * l + 1) * fac(l - abs(m)) /
                    (4.0 * PI * fac(l + abs(m))));
  if (m > 0) {
    return sqrt(2.0) * kml * cos(m * phi) *
        legendre(cos(theta), m, l);
  } else if (m < 0) {
    return sqrt(2.0) * kml * sin(-m * phi) *
        legendre(cos(theta), -m, l);
  } else {
    return kml * legendre(cos(theta), 0, l);
  }
}

float SHFunction::sh(float theta, float phi, int m, int l)
{
    if(m == 0)
    {
        return K(0, l) * legendre(cos(theta), 0, l);
    }
    if(m > 0)
    {
        return sqrt(2.0) * K(m, l) * cos(m * phi) * legendre(cos(theta), m, l);
    }
    return sqrt(2.0) * K(m, l) * sin(-m * phi) * legendre(cos(theta), -m, l);
}

float SHFunction::doubleFac(int n)
{
    if(n <= 1)
    {
        return 1.0f;
    }
    return (float)n * doubleFac(n - 2);
}

float SHFunction::fac(int n)
{
    if(n <= 1)
    {
        return 1;
    }
    return (float)n * fac(n - 1);
}

float SHFunction::K(int m, int l)
{
    return sqrt((2.0 * (float)l + 1.0) * fac(l - abs(m)) / (4.0 * PI * fac(l + abs(m))));
}

float SHFunction::legendre(float x, int m, int l)
{
    if(l == m + 1)
    {
        return x * (2.0f * (float)m + 1.0) * legendre(x, m, m);
    }
    if(l == m)
    {
        return pow(-1, m) * doubleFac(2 * m - 1) * pow(1.0f - x * x, (float)m / 2.0);
    }
    return (x * (2.0f * l - 1.0f) * legendre(x, m, l - 1) - (float)(l + m - 1) * legendre(x, m, l - 2)) / (float)(l - m);
    // double pmm = 1.0;
    // if (m > 0) {
    //     double sign = (m % 2 == 0 ? 1 : -1);
    //     pmm = sign * doubleFac(2 * m - 1) * pow(1 - x * x, m / 2.0);
    // }

    // if (l == m) {
    //     return pmm;
    // }

    // double pmm1 = x * (2 * m + 1) * pmm;
    // if (l == m + 1) {
    //     return pmm1;
    // }

    // for (int n = m + 2; n <= l; n++) {
    //     double pmn = (x * (2 * n - 1) * pmm1 - (n + m - 1) * pmm) / (n - m);
    //     pmm = pmm1;
    //     pmm1 = pmn;
    // }
    // return pmm1;
}
