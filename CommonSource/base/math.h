#pragma once
#include <algorithm>
#include <cmath>
#include <limits>

namespace math {

constexpr double eps = 1E-10;
constexpr double pi = 3.1415926535897931;    // 4.0*std::atan(1.0);
constexpr double toDegree = 180.0 / pi;
constexpr double toRadian = pi / 180.0;

#if defined(ANDROID) || defined(__APPLE__) || defined(_MSC_VER)
inline bool equal(double d0, double d1, double tolerance = math::eps)
#else
constexpr bool equal(double d0, double d1, double tolerance = math::eps)
#endif
{
    return std::abs(d0 - d1) <= tolerance;
}

template<typename Scalar0, typename Scalar1>
inline double ratio(Scalar0 s0, Scalar1 s1)
{
    return static_cast<double>(s0) / static_cast<double>(s1);
}

template<typename T>
inline double maxRatio(T t0, T t1)
{
    if (t0 == 0 || t1 == 0)
        return std::numeric_limits<double>::infinity();
    return std::max(ratio(t0, t1), ratio(t1, t0));
}

}    // namespace math
