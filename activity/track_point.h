#pragma once

#include <chrono>
#include <string>


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
    bool operator==(const TrackPoint& other) const
    {
        return time == other.time &&
                latitude == other.latitude &&
                longitude == other.longitude &&
                altitude == other.altitude &&
                heartRate == other.heartRate &&
                startOfSegement == other.startOfSegement;
    }
    bool operator!=(const TrackPoint& other) const
    {
        return !operator==(other);
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
    Displacement(std::chrono::system_clock::duration duration, double hDistance, double vDistance)
        :
        duration(duration),
        hDistance(hDistance),
        vDistance(vDistance)
    {
    }
    std::chrono::system_clock::duration duration;
    double hDistance;
    double vDistance;
    double grade() const
    {
        if (hDistance == 0)
        {
            return 0;
        }

        return vDistance / hDistance;
    }
    double speed() const
    {
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

        if (seconds == 0)
        {
            return 0;
        }

        return hDistance / seconds;
    }
};

Displacement operator-(const TrackPoint& a, const TrackPoint &b);
