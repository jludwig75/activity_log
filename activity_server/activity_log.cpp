#include "activity_log.h"


using grpc::Status;


grpc::Status ActivityLog::uploadActivity(grpc::ServerContext* context,
                            grpc::ServerReader<activity_log::ActivityFileChunk>* stream,
                            activity_log::Activity* activity)
{
    return Status::OK;
}


grpc::Status ActivityLog::getActivityList(grpc::ServerContext* context,
                                const activity_log::TimeSpan* timesSpan,
                                grpc::ServerWriter<activity_log::Activity>* stream)
{
    return Status::OK;
}

grpc::Status ActivityLog::calculateStats(grpc::ServerContext* context,
                            const activity_log::TimeSpan* timesSpan,
                            activity_log::Stats* stats)
{
    return Status::OK;
}


grpc::Status ActivityLog::getActivity(grpc::ServerContext* context,
                            const activity_log::ActivityRequest* request,
                            activity_log::Activity* activity)
{
    return Status::OK;
}


// For generating analysis plots
grpc::Status ActivityLog::plotActivity(grpc::ServerContext* context,
                            const activity_log::ActivityRequest*,
                            grpc::ServerWriter<activity_log::DataPoint>* stream)
{
    return Status::OK;
}

// For creating activity maps/profiles
grpc::Status ActivityLog::getActivityTrack(grpc::ServerContext* context,
                                const activity_log::ActivityRequest*,
                                grpc::ServerWriter<activity_log::TrackPoint>* stream)
{
    return Status::OK;
}


grpc::Status ActivityLog::editActivity(grpc::ServerContext* context,
                            const activity_log::EditActivityRequest*,
                            activity_log::Activity*)
{
    return Status::OK;
}


grpc::Status ActivityLog::deleteActivity(grpc::ServerContext* context,
                            const activity_log::ActivityRequest*,
                            activity_log::Empty* )
{
    return Status::OK;
}


grpc::Status ActivityLog::downloadActivity(grpc::ServerContext* context,
                                const activity_log::ActivityRequest*,
                                grpc::ServerWriter<activity_log::ActivityFileChunk>* stream)
{
    return Status::OK;
}

