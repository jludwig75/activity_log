#pragma once

#include <chrono>


class TrackPoint
{
public:
    std::chrono::system_clock::time_point time;
    double latitude;
    double longitude;
    float altitude;
};