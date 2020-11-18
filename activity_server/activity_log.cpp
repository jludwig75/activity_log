#include "activity_log.h"

#include <grpcpp/impl/codegen/status_code_enum.h>

#include "activity.h"
#include "gpxfile.h"


using grpc::Status;


grpc::Status ActivityLog::uploadActivity(grpc::ServerContext* context,
                            grpc::ServerReader<activity_log::ActivityFileChunk>* stream,
                            activity_log::Activity* activity)
{
    std::string activityData;

    activity_log::ActivityFileChunk chunk;
    while(stream->Read(&chunk))
    {
        activityData += chunk.data();
    }

    Activity newActivity;
    if (!_parser.parseActivityData(activityData, newActivity))
    {
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Error parsing GPX data");
    }

    if (newActivity.name.empty())
    {
        // TODO: Maybe add the time or time of day to the name
        newActivity.name = "New Activity";
    }

    if (!_db.storeActivity(newActivity, newActivity.id))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error storing activity");
    }

    toProto(newActivity, activity);

    return Status::OK;
}


grpc::Status ActivityLog::getActivityList(grpc::ServerContext* context,
                                const activity_log::TimeSpan* timesSpan,
                                grpc::ServerWriter<activity_log::Activity>* stream)
{
    ActivityDatabase::ActivityMap activities;
    // TODO: Add support for filtering by timespan
    if (!_db.listActivities(activities, false))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error getting activity map");
    }

    for (auto& [activityId, activity]: activities)
    {
        activity_log::Activity protoActivity;
        toProto(activity, &protoActivity);
        if (!stream->Write(std::move(protoActivity)))
        {
            return grpc::Status(grpc::StatusCode::UNKNOWN, "Error writing activity to stream");
        }
    }

    return Status::OK;
}

grpc::Status ActivityLog::calculateStats(grpc::ServerContext* context,
                            const activity_log::TimeSpan* timesSpan,
                            activity_log::Stats* stats)
{
    ActivityDatabase::ActivityMap activities;
    // TODO: Add support for filtering by timespan
    if (!_db.listActivities(activities))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error getting activity map");
    }

    stats->set_total_activities(0);
    stats->set_total_time(0);
    stats->set_total_distance(0);
    stats->set_total_ascent(0);
    for (auto& [activityId, activity]: activities)
    {
        stats->set_total_activities(stats->total_activities() + 1);
        stats->set_total_time(stats->total_time() + std::chrono::duration_cast<std::chrono::seconds>(activity.duration()).count());
        stats->set_total_distance(stats->total_distance() + activity.stats.total_distance);
        stats->set_total_ascent(stats->total_ascent() + activity.stats.total_ascent);
    }

    return Status::OK;
}


grpc::Status ActivityLog::getActivity(grpc::ServerContext* context,
                            const activity_log::ActivityRequest* request,
                            activity_log::Activity* activity)
{
    Activity retrievedActivity;
    if (!_db.loadActivity(request->activity_id(), retrievedActivity))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error getting activity " + request->activity_id());
    }

    toProto(retrievedActivity, activity);

    return Status::OK;
}


// For generating analysis plots
grpc::Status ActivityLog::plotActivity(grpc::ServerContext* context,
                            const activity_log::ActivityRequest* request,
                            grpc::ServerWriter<activity_log::DataPoint>* stream)
{
    Activity activity;
    if (!_db.loadActivity(request->activity_id(), activity))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error getting activity " + request->activity_id());
    }

    TrackPoint previousPoint;
    double cummulativeDistance = 0;
    float cummulativeAscent = 0;
    float cummulativeDescent = 0;
    auto startTime = std::chrono::system_clock::from_time_t(0);
    for (const auto& trackPoint : activity.trackPoints)
    {
        if (startTime == std::chrono::system_clock::from_time_t(0))
        {
            startTime = trackPoint.time;
        }
        else
        {
            assert(trackPoint.time >= previousPoint.time);
            auto displacement = trackPoint - previousPoint;

            cummulativeDistance += displacement.hDistance;

            if (displacement.vDistance > 0)
            {
                cummulativeAscent += displacement.vDistance;
            }
            else if (displacement.vDistance < 0)
            {
                cummulativeDescent += -displacement.vDistance;
            }

            activity_log::DataPoint dataPoint;
            dataPoint.set_time(std::chrono::duration_cast<std::chrono::seconds>(trackPoint.time - startTime).count());
            dataPoint.set_altitude(trackPoint.altitude);
            dataPoint.set_cummulative_distance(cummulativeDistance);
            dataPoint.set_speed(displacement.speed());
            dataPoint.set_grade(displacement.grade());
            dataPoint.set_heart_rate(trackPoint.heartRate);
            dataPoint.set_cummulative_ascent(cummulativeAscent);
            dataPoint.set_cummulative_descent(cummulativeDescent);

            if (!stream->Write(dataPoint))
            {
                return grpc::Status(grpc::StatusCode::UNKNOWN, "Error writing data point to stream");
            }
        }

        previousPoint = trackPoint;
    }

    return Status::OK;
}

// For creating activity maps/profiles
grpc::Status ActivityLog::getActivityTrack(grpc::ServerContext* context,
                                const activity_log::ActivityRequest* request,
                                grpc::ServerWriter<activity_log::TrackPoint>* stream)
{
    Activity activity;
    if (!_db.loadActivity(request->activity_id(), activity))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error getting activity " + request->activity_id());
    }

    for (const auto& trackPoint : activity.trackPoints)
    {
        activity_log::TrackPoint tp;

        tp.set_time(std::chrono::system_clock::to_time_t(trackPoint.time));
        tp.set_latitude(trackPoint.latitude);
        tp.set_longitude(trackPoint.longitude);
        tp.set_altitude(trackPoint.altitude);
        tp.set_start_of_sgement(trackPoint.startOfSegement);

        if (!stream->Write(tp))
        {
            return grpc::Status(grpc::StatusCode::UNKNOWN, "Error writing activity to stream");
        }
    }

    return Status::OK;
}


grpc::Status ActivityLog::editActivity(grpc::ServerContext* context,
                            const activity_log::EditActivityRequest* request,
                            activity_log::Activity* activity)
{
    auto newName = request->name();

    // Fail if not fields are set to be changed
    if (newName.empty())
    {
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "No fields were specified for edit");
    }

    Activity retrievedActivity;
    if (!_db.loadActivity(request->activity_id(), retrievedActivity))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error getting activity " + request->activity_id());
    }

    retrievedActivity.name = newName;
    if (!_db.updateActivity(request->activity_id(), retrievedActivity))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error editing activity " + request->activity_id());
    }

    return Status::OK;
}


grpc::Status ActivityLog::deleteActivity(grpc::ServerContext* context,
                            const activity_log::ActivityRequest* request,
                            activity_log::Empty* )
{
    if (!_db.deleteActivity(request->activity_id()))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error deleting activity " + request->activity_id());
    }

    return Status::OK;
}


grpc::Status ActivityLog::downloadActivity(grpc::ServerContext* context,
                                const activity_log::ActivityRequest* request,
                                grpc::ServerWriter<activity_log::ActivityFileChunk>* stream)
{
    Activity activity;
    if (!_db.loadActivity(request->activity_id(), activity))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error getting activity " + request->activity_id());
    }

    std::string gpxFileData;
    if (!gpxfile::generateFileData(activity, gpxFileData))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error generating GPX data for activity");
    }

    // stream data back
    std::string::size_type offset = 0;
    while (offset < gpxFileData.length())
    {
        activity_log::ActivityFileChunk chunk;
        std::string chunkData = gpxFileData.substr(offset, activity_log::ActivityFileChunk::Limits::ActivityFileChunk_Limits_MAX_CHUNK_SIZE);
        offset += chunkData.length();
        chunk.set_data(chunkData);
        if (!stream->Write(std::move(chunk)))
        {
            break;
        }
    }

    return Status::OK;
}

void ActivityLog::toProto(const Activity& activity, activity_log::Activity* protoActivity) const
{
    // id
    protoActivity->set_id(activity.id);
    // name
    protoActivity->set_name(activity.name);
    // start_time
    protoActivity->set_start_time(std::chrono::system_clock::to_time_t(activity.start_time));
    // duration
    protoActivity->set_duration(std::chrono::duration_cast<std::chrono::seconds>(activity.duration()).count());
    // moving_time
    protoActivity->set_moving_time(std::chrono::duration_cast<std::chrono::seconds>(activity.stats.movingTime).count());
    // total_distance
    protoActivity->set_total_distance(activity.stats.total_distance);
    // total_ascent
    protoActivity->set_total_ascent(activity.stats.total_ascent);
    // total_descent
    protoActivity->set_total_descent(activity.stats.total_descent);
    // max_speed
    protoActivity->set_max_speed(activity.stats.max_speed);
    // average_heart_rate
    protoActivity->set_average_heart_rate(static_cast<uint32_t>(activity.stats.average_heart_rate));
    // max_heart_rate
    protoActivity->set_max_heart_rate(activity.stats.max_heart_rate);
    // average_climbing_grade
    protoActivity->set_average_climbing_grade(activity.stats.average_climbing_grade);
    // average_descending_grade
    protoActivity->set_average_descending_grade(activity.stats.average_descending_grade);
}
