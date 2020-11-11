#pragma once

#include <chrono>


class TrackPoint
{
public:
    TrackPoint() = default;
    TrackPoint(std::chrono::system_clock::time_point time,
               double latitude,
               double longitude,
               float altitude,
               bool startOfSegement)
        :
        time(time),
        latitude(latitude),
        longitude(longitude),
        altitude(altitude),
        startOfSegement(startOfSegement)
    {

    }
    std::chrono::system_clock::time_point time;
    double latitude;
    double longitude;
    float altitude;
    bool startOfSegement;
};