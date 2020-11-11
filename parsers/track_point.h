#pragma once

#include <chrono>
#include <cmath>


class TrackPoint
{
public:
    TrackPoint() = default;
    TrackPoint(std::chrono::system_clock::time_point time,
               double latitude,
               double longitude,
               float altitude,
               unsigned heartRate,
               bool startOfSegement)
        :
        time(time),
        latitude(latitude),
        longitude(longitude),
        altitude(altitude),
        heartRate(heartRate),
        startOfSegement(startOfSegement)
    {
    }
    std::chrono::system_clock::time_point time;
    double latitude;
    double longitude;
    float altitude;
    unsigned heartRate;
    bool startOfSegement;
};


class Displacement
{
public:
    Displacement(double hDistance, double vDistance)
        :
        hDistance(hDistance),
        vDistance(vDistance)
    {
    }
    double hDistance;
    double vDistance;
};

Displacement operator-(const TrackPoint& a, const TrackPoint &b);
