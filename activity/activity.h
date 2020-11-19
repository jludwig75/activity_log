#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "track_point.h"


class Activity
{
public:

    bool operator==(const Activity& other) const;
    bool operator!=(const Activity& other) const;

    void analyzeTrackPoints();

    std::string id;
    std::string name;
    std::chrono::system_clock::time_point start_time;
    std::vector<TrackPoint> trackPoints;
    struct ActivityStats
    {
        bool operator==(const ActivityStats& other) const
        {
            return total_distance == other.total_distance &&
                total_ascent == other.total_ascent &&
                total_descent == other.total_descent &&
                max_speed == other.max_speed &&
                average_heart_rate == other.average_heart_rate &&
                max_heart_rate == other.max_heart_rate &&
                average_climbing_grade == other.average_climbing_grade &&
                average_descending_grade == other.average_descending_grade &&
                movingTime == other.movingTime;
        }
        bool operator!=(const ActivityStats& other) const
        {
            return !operator==(other);
        }
        float total_distance;
        float total_ascent;
        float total_descent;
        float max_speed;
        double average_heart_rate;
        uint32_t max_heart_rate;
        double average_climbing_grade;
        double average_descending_grade;
        std::chrono::system_clock::duration movingTime;
        std::chrono::system_clock::duration duration;
        double startLongitude;
        double startLatitude;
    } stats{};
};