#include "activity.h"

#include <cassert>


bool Activity::operator==(const Activity& other) const
{
    if (name != other.name || start_time != other.start_time || stats != other.stats)
    {
        return false;
    }

    if (trackPoints.size() != other.trackPoints.size())
    {
        return false;
    }

    for (size_t i = 0; i < trackPoints.size(); ++i)
    {
        if (trackPoints[i] != other.trackPoints[i])
        {
            return false;
        }
    }

    return true;
}

bool Activity::operator!=(const Activity& other) const
{
    return !operator==(other);
}



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
    if (trackPoints.empty())
    {
        return;
    }

    stats = {};
    TrackPoint previous;
    unsigned intervalCount = 0;

    auto activityStartTime = trackPoints.front().time;
    for (const auto& trackPoint : trackPoints)
    {
        if (trackPoint.time > activityStartTime)
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
            // average over time
            auto previousRelativeStart = (previous.time -  activityStartTime).count();          // current average time span
            auto trackPointRelativeStart = (trackPoint.time - activityStartTime).count();       // new average time span
            auto trackPointDuration = (trackPoint.time - previous.time).count();                // duration of current track point
            double averageTrackPointHeartRate = (static_cast<double>(previous.heartRate) + trackPoint.heartRate) / 2.0;  // average over current trackpoint 
            stats.average_heart_rate = (stats.average_heart_rate * previousRelativeStart + averageTrackPointHeartRate * trackPointDuration) / trackPointRelativeStart;

            // max HR
            if (trackPoint.heartRate > stats.max_heart_rate)
            {
                stats.max_heart_rate = trackPoint.heartRate;
            }
        }
        else if (trackPoint.time ==  activityStartTime)
        {
            stats.average_heart_rate = trackPoint.heartRate;
        }

        previous = trackPoint;
    }
}
