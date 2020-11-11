#include "activity_log.h"

#include <fstream>

#include <grpcpp/client_context.h>


#define MAX_DATA_CHUNCK_SIZE    (64 * 1024)


using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::ClientReader;
using grpc::Status;


namespace
{

std::string readFile(const std::string& fileName)
{
    std::ifstream t(fileName);
    std::string str;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    return str;
}

}

ActivityLog::ActivityLog(std::shared_ptr<grpc::Channel> channel)
    :
    _stub(activity_log::ActivityLog::NewStub(channel))
{
}

grpc::Status ActivityLog::uploadActivity(const std::string& activityFileName, Activity& newActivity)
{
    ClientContext context;

    // TODO: This is naive: It lacks error handling and may not handle binary data (i.e. FIT files)
    std::string activityFileData = readFile(activityFileName);

    activity_log::Activity activity;
    std::unique_ptr<grpc::ClientWriter<activity_log::ActivityFileChunk> > stream(_stub->uploadActivity(&context, &activity));

    std::string::size_type offset = 0;
    while (offset < activityFileData.length())
    {
        activity_log::ActivityFileChunk chunk;
        std::string chunkData = activityFileData.substr(offset, MAX_DATA_CHUNCK_SIZE);
        offset += chunkData.length();
        chunk.set_data(chunkData);
        if (!stream->Write(std::move(chunk)))
        {
            break;
        }
    }
    stream->WritesDone();

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
