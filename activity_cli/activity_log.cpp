#include "activity_log.h"

#include "threadfile.h"

#include <thread>

#include <grpcpp/client_context.h>


using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::ClientReader;
using grpc::Status;


ActivityLog::ActivityLog(std::shared_ptr<grpc::Channel> channel)
    :
    _stub(activity_log::ActivityLog::NewStub(channel))
{
}

grpc::Status ActivityLog::uploadActivity(const std::string& activityFileName, Activity& newActivity)
{
    ClientContext context;

    Container<threadfile::FileChunk> fileChunks;
    bool readerSuccess = false;
    std::thread reader([activityFileName, &fileChunks, &readerSuccess]{
        readerSuccess = threadfile::readFile(activityFileName, activity_log::ActivityFileChunk::Limits::ActivityFileChunk_Limits_MAX_CHUNK_SIZE, fileChunks);
    });

    activity_log::Activity activity;
    std::unique_ptr<grpc::ClientWriter<activity_log::ActivityFileChunk> > stream(_stub->uploadActivity(&context, &activity));

    for (const auto& fileChunk : fileChunks)
    {
        activity_log::ActivityFileChunk chunk;
        chunk.set_data(fileChunk.data(), fileChunk.size());
        if (!stream->Write(std::move(chunk)))
        {
            break;
        }
    }
    stream->WritesDone();
    reader.join();
    if (!readerSuccess)
    {
        return grpc::Status(grpc::StatusCode::UNKNOWN, "Error reading GPX file");
    }

    auto status = stream->Finish();

    if (status.ok())
    {
        newActivity = Activity(activity, _stub);
    }

    return status;
}

grpc::Status ActivityLog::listActivities(system_clock::time_point startTime, system_clock::time_point endTime, std::vector<Activity> &activities) const
{
    ClientContext context;

    activity_log::TimeSpan timeSpan;
    timeSpan.set_start_time(system_clock::to_time_t(startTime));
    timeSpan.set_end_time(system_clock::to_time_t(endTime));

    std::unique_ptr<grpc::ClientReader<activity_log::Activity> > stream(_stub->getActivityList(&context, timeSpan));

    activity_log::Activity activity;
    while (stream->Read(&activity))
    {
        activities.emplace_back(activity, _stub);
    }

    return stream->Finish();
}

grpc::Status ActivityLog::calculateStats(system_clock::time_point startTime, system_clock::time_point endTime, activity_log::Stats& stats) const
{
    ClientContext context;

    activity_log::TimeSpan timeSpan;
    timeSpan.set_start_time(system_clock::to_time_t(startTime));
    timeSpan.set_end_time(system_clock::to_time_t(endTime));

    return _stub->calculateStats(&context, timeSpan, &stats);
}


grpc::Status ActivityLog::getActivity(const std::string& activityId, Activity& activity) const
{
    ClientContext context;

    activity_log::ActivityRequest request;
    request.set_activity_id(activityId);

    activity_log::Activity restrievedActivity;
    auto status = _stub->getActivity(&context, request, &restrievedActivity);

    if (status.ok())
    {
        activity = Activity(restrievedActivity, _stub);
    }

    return status;
}
