#include "activity.h"


std::chrono::system_clock::time_point Activity::start_time() const
{
    if (trackPoints.empty())
    {
        return std::chrono::system_clock::from_time_t(0);
    }
    return trackPoints.front().time;
}
std::chrono::system_clock::duration Activity::duration() const
{
    if (trackPoints.empty())
    {
        return std::chrono::seconds(0);
    }
    return trackPoints.back().time - trackPoints.front().time;
}

std::string Activity::name() const
{
    if (!_name.empty() || trackPoints.empty())
    {
        return _name;
    }

    if (!trackPoints[0].activityName.empty())
    {
        _name = trackPoints[0].activityName;
        const_cast<Activity*>(this)->trackPoints[0].activityName = "";
    }
    return _name;
}

void Activity::set_name(const std::string& name) const
{
    _name = name;
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
