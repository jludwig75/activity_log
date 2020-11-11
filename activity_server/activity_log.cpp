#include "activity_log.h"

#include <grpcpp/impl/codegen/status_code_enum.h>

#include "activity.h"


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
    if (!_parser.parseActivityData(activityData, newActivity.trackPoints))
    {
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Error parsing GPX data");
    }

    newActivity.name = "New Activity";
    newActivity.analyzeTrackPoints();

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
    if (!_db.listActivities(activities))
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
    return Status::OK;
}

// For creating activity maps/profiles
grpc::Status ActivityLog::getActivityTrack(grpc::ServerContext* context,
                                const activity_log::ActivityRequest* request,
                                grpc::ServerWriter<activity_log::TrackPoint>* stream)
{
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
    Activity retrievedActivity;
    if (!_db.deleteActivity(request->activity_id()))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error deleting activity " + request->activity_id());
    }

    return Status::OK;
}


grpc::Status ActivityLog::downloadActivity(grpc::ServerContext* context,
                                const activity_log::ActivityRequest*,
                                grpc::ServerWriter<activity_log::ActivityFileChunk>* stream)
{
    return Status::OK;
}

void ActivityLog::toProto(const Activity& activity, activity_log::Activity* protoActivity) const
{
    // id
    protoActivity->set_id(activity.id);
    // name
    protoActivity->set_name(activity.name);
    // start_time
    protoActivity->set_start_time(std::chrono::system_clock::to_time_t(activity.start_time()));
    // duration
    protoActivity->set_duration(std::chrono::duration_cast<std::chrono::seconds>(activity.duration()).count());
    // total_distance
    protoActivity->set_total_distance(activity.stats.total_distance);
    // total_ascent
    protoActivity->set_total_ascent(activity.stats.total_ascent);
    // total_descent
    protoActivity->set_total_descent(activity.stats.total_descent);
    // max_speed
    protoActivity->set_max_speed(activity.stats.max_speed);
    // average_heart_rate
    protoActivity->set_average_heart_rate(activity.stats.average_heart_rate);
    // max_heart_rate
    protoActivity->set_max_heart_rate(activity.stats.max_heart_rate);
    // average_climbing_grade
    protoActivity->set_average_climbing_grade(activity.stats.average_climbing_grade);
    // average_descending_grade
    protoActivity->set_average_descending_grade(activity.stats.average_descending_grade);
}
