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
               bool startOfSegement,
               bool endOfSegement)
        :
        time(time),
        latitude(latitude),
        longitude(longitude),
        altitude(altitude),
        startOfSegement(startOfSegement),
        endOfSegement(endOfSegement)
    {

    }
    std::chrono::system_clock::time_point time;
    double latitude;
    double longitude;
    float altitude;
    bool startOfSegement;
    bool endOfSegement;
};