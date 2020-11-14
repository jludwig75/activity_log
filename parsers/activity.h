#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "track_point.h"


class Activity
{
public:
    void analyzeTrackPoints();

    std::string id;
    std::string name;
    std::chrono::system_clock::time_point start_time;
    std::chrono::system_clock::duration duration() const;
    std::vector<TrackPoint> trackPoints;
    struct ActivityStats
    {
        float total_distance;
        float total_ascent;
        float total_descent;
        float max_speed;
        uint32_t average_heart_rate;
        uint32_t max_heart_rate;
        double average_climbing_grade;
        double average_descending_grade;
    } stats{};
};