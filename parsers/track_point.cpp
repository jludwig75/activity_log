#include "track_point.h"

#include <cmath>


namespace
{

template<typename ValueType>
ValueType radians(ValueType v)
{
    return (v * M_PI) / 180.0;
}

double greatCircleDistance(const TrackPoint& pos1, const TrackPoint& pos2)
{
    auto phi1 = radians(pos1.latitude);
    auto phi2 = radians(pos2.latitude);
    auto deltaPhi = radians(pos2.latitude - pos1.latitude);
    auto deltaLambda = radians(pos2.longitude - pos1.longitude);
    auto a = std::sin(deltaPhi/2) * std::sin(deltaPhi/2) + std::cos(phi1) * std::cos(phi2) * std::sin(deltaLambda / 2) * std::sin(deltaLambda / 2);
    auto c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    return 6371000 * c;
}

}

Displacement operator-(const TrackPoint& a, const TrackPoint &b)
{
    return Displacement(greatCircleDistance(a, b), a.altitude - b.altitude);
}