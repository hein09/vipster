#ifndef VIPSTER_DEFINITIONS_H
#define VIPSTER_DEFINITIONS_H

#include <array>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <cmath>

namespace Vipster{
    constexpr float bohrrad = 0.52917721092;
    constexpr float invbohr = 1/bohrrad;

    enum class Fmt { Bohr = 1, Angstrom = 2, Crystal = 3, Alat = 4 };

    typedef std::array<float,3> Vec;
    bool  operator==(const Vec &v1, const Vec &v2);
    Vec operator+=(Vec &v1, const Vec &v2);
    Vec operator+(Vec v1, const Vec &v2);
    Vec operator-(const Vec &v);
    Vec operator-=(Vec &v1, const Vec &v2);
    Vec operator-(Vec v1, const Vec &v2);
    Vec operator*=(Vec &v, const float &f);
    Vec operator*(Vec v, const float &f);
    Vec operator/=(Vec &v, const float &f);
    Vec operator/(Vec v, const float &f);
    float t_vec_length(const Vec &v);
    float t_vec_dot(const Vec &v1, const Vec &v2);
    Vec t_vec_cross(const Vec &v1, const Vec &v2);

    struct Atom{
        std::string name;
        Vec coord;
        float charge;
        std::array<bool,3> fix;
        bool hidden;
    };

    struct Bond{
        ulong at1;
        ulong at2;
        float dist;
    };
}

#endif // VIPSTER_DEFINITIONS_H
