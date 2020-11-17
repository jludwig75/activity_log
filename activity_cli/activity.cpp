#include "activity.h"

#include <thread>

#include "threadfile.h"

using grpc::ClientContext;
using grpc::Status;

Activity::Activity()
    :
    _defunc(true)   // TODO: Check _defunc before very operation
{
}

Activity::Activity(activity_log::Activity& activity, std::shared_ptr<activity_log::ActivityLog::Stub> stub)
    :
    _defunc(false),
    _activity(activity),
    _stub(stub)
{
}

const std::string& Activity::id() const
{
    return _activity.id();
}

const std::string& Activity::name() const
{
    return _activity.name();
}

std::chrono::system_clock::time_point Activity::startTime() const
{
    return std::chrono::system_clock::from_time_t(static_cast<std::time_t>(_activity.start_time()));
}

std::chrono::system_clock::duration Activity::duration() const
{
    return std::chrono::seconds(_activity.duration());
}

std::chrono::system_clock::duration Activity::movingTime() const
{
    return std::chrono::seconds(_activity.moving_time());
}


float Activity::totalDistance() const
{
    return _activity.total_distance();
}

float Activity::totalAscent() const
{
    return _activity.total_ascent();
}

float Activity::totalDescent() const
{
    return _activity.total_descent();
}

float Activity::average_speed() const
{
    auto duration_ns = std::chrono::duration<float>(duration());
    auto duration_seconds = std::chrono::duration_cast<std::chrono::seconds>(duration_ns).count();
    if (duration_seconds == 0)
    {
        return 0;
    }

    return totalDistance() / duration_seconds;
}

float Activity::moving_average_speed() const
{
    auto duration_ns = std::chrono::duration<float>(movingTime());
    auto duration_seconds = std::chrono::duration_cast<std::chrono::seconds>(duration_ns).count();
    if (duration_seconds == 0)
    {
        return 0;
    }

    return totalDistance() / duration_seconds;
}

float Activity::max_speed() const
{
    return _activity.max_speed();
}

uint32_t Activity::average_heart_rate() const
{
    return _activity.average_heart_rate();
}

uint32_t Activity::max_heart_rate() const
{
    return _activity.max_heart_rate();
}

float Activity::average_climbing_grade() const
{
    return _activity.average_climbing_grade();
}

float Activity::average_descending_grade() const
{
    return _activity.average_descending_grade();
}

grpc::Status Activity::edit(const std::string& newName)
{
    ClientContext context;

    activity_log::EditActivityRequest editRequest;
    editRequest.set_activity_id(_activity.id());
    editRequest.set_name(newName);

    activity_log::Activity activity;
    auto status = _stub->editActivity(&context, editRequest, &activity);

    if (status.ok())
    {
        _activity = activity;
    }

    return status;
}

grpc::Status Activity::del()
{
    ClientContext context;

    auto request = activityRequest();

    activity_log::Empty response;
    auto status = _stub->deleteActivity(&context, request, &response);
    if (status.ok())
    {
        _defunc = true;
    }

    return status;
}

grpc::Status Activity::plot(std::vector<activity_log::DataPoint>& dataPoints) const
{
    ClientContext context;

    auto request = activityRequest();

    std::unique_ptr<grpc::ClientReader<activity_log::DataPoint> > stream(_stub->plotActivity(&context, request));
    
    activity_log::DataPoint dataPoint;
    while (stream->Read(&dataPoint))
    {
        dataPoints.emplace_back(std::move(dataPoint));
    }

    return stream->Finish();
}

grpc::Status Activity::getTracks(std::vector<activity_log::TrackPoint>& trackPoints) const
{
    ClientContext context;

    auto request = activityRequest();

    std::unique_ptr<grpc::ClientReader<activity_log::TrackPoint> > stream(_stub->getActivityTrack(&context, request));
    
    activity_log::TrackPoint trackPoint;
    while (stream->Read(&trackPoint))
    {
        trackPoints.emplace_back(std::move(trackPoint));
    }

    return stream->Finish();
}

grpc::Status Activity::downloadToGpx(const std::string& gpxFileName) const
{
    ClientContext context;

    auto request = activityRequest();

    std::unique_ptr<grpc::ClientReader<activity_log::ActivityFileChunk> > stream(_stub->downloadActivity(&context, request));
    
    InterThreadQueue<threadfile::FileChunk> fileChunks;
    std::thread downloader([&stream, &fileChunks]{
        activity_log::ActivityFileChunk chunk;
        while (stream->Read(&chunk))
        {
            auto chunkData = chunk.data();
            threadfile::FileChunk fileChunk(chunkData.size());
            memcpy(fileChunk.data(), chunkData.data(), chunkData.size());
            fileChunk.setSize(chunkData.size());

            fileChunks.push(std::move(fileChunk));
        }
        fileChunks.done_pushing();
    });

    if (!threadfile::writeFile(gpxFileName, fileChunks))
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error writing to GPX file");
    }

    downloader.join();

    return stream->Finish();
}

activity_log::ActivityRequest Activity::activityRequest() const
{
    activity_log::ActivityRequest request;
    request.set_activity_id(_activity.id());

    return request;
}
