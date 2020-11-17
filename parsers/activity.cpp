#include "activity.h"

#include <cassert>


std::chrono::system_clock::duration Activity::duration() const
{
    if (trackPoints.empty())
    {
        return std::chrono::seconds(0);
    }
    return trackPoints.back().time - start_time;
}

void Activity::analyzeTrackPoints()
{
    stats = {};
    TrackPoint previous;
    unsigned intervalCount = 0;

    for (const auto& trackPoint : trackPoints)
    {
        if (trackPoint.time > trackPoints.front().time)
        {
            intervalCount++;
            assert(trackPoint.time >= previous.time);
            auto displacement = trackPoint - previous;
            auto duration = displacement.duration;
            
            // distance
            stats.total_distance += displacement.hDistance;

            if (displacement.vDistance > 0)
            {
                // ascent
                stats.total_ascent += displacement.vDistance;

                // average climbing grade
                if (displacement.hDistance > 0)
                {
                    stats.average_climbing_grade = (stats.average_climbing_grade * (intervalCount - 1) + displacement.grade()) / intervalCount;
                }
            }
            else if (displacement.vDistance < 0)
            {
                // descent
                stats.total_descent += -displacement.vDistance;

                // average descending grade
                if (displacement.hDistance > 0)
                {
                    stats.average_descending_grade = (stats.average_descending_grade * (intervalCount - 1) + displacement.grade()) / intervalCount;
                }
            }

            // max speed
            auto speed = displacement.speed();
            if (speed > stats.max_speed)
            {
                stats.max_speed = speed;
            }

            if (speed > 0.3048) // Have to move at least a foot in 1 second to be considered moving
            {
                stats.movingTime += trackPoint.time - previous.time;
            }


            // average heart rate
            // cast to uint64_t for calculation to prevent overflow.
            stats.average_heart_rate = static_cast<uint32_t>((static_cast<uint64_t>(stats.average_heart_rate) * (intervalCount - 1) + trackPoint.heartRate) / intervalCount);

            // max HR
            if (trackPoint.heartRate > stats.max_heart_rate)
            {
                stats.max_heart_rate = trackPoint.heartRate;
            }
        }

        previous = trackPoint;
    }
}
