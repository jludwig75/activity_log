#pragma once

#include <grpc/grpc.h>
#include <grpcpp/server.h>

#include "activity_log.grpc.pb.h"

#include "parser.h"

class ActivityLog final : public activity_log::ActivityLog::Service
{
public:
    grpc::Status uploadActivity(grpc::ServerContext* context,
                                grpc::ServerReader<activity_log::ActivityFileChunk>* stream,
                                activity_log::Activity* activity) override;

    grpc::Status getActivityList(grpc::ServerContext* context,
                                 const activity_log::TimeSpan* timesSpan,
                                 grpc::ServerWriter<activity_log::Activity>* stream) override;

    grpc::Status calculateStats(grpc::ServerContext* context,
                                const activity_log::TimeSpan* timesSpan,
                                activity_log::Stats* stats) override;

    grpc::Status getActivity(grpc::ServerContext* context,
                             const activity_log::ActivityRequest* request,
                             activity_log::Activity* activity) override;

    // For generating analysis plots
    grpc::Status plotActivity(grpc::ServerContext* context,
                              const activity_log::ActivityRequest*,
                              grpc::ServerWriter<activity_log::DataPoint>* stream) override;

    // For creating activity maps/profiles
    grpc::Status getActivityTrack(grpc::ServerContext* context,
                                  const activity_log::ActivityRequest*,
                                  grpc::ServerWriter<activity_log::TrackPoint>* stream) override;

    grpc::Status editActivity(grpc::ServerContext* context,
                              const activity_log::EditActivityRequest*,
                              activity_log::Activity*) override;

    grpc::Status deleteActivity(grpc::ServerContext* context,
                                const activity_log::ActivityRequest*,
                                activity_log::Empty* ) override;

    grpc::Status downloadActivity(grpc::ServerContext* context,
                                  const activity_log::ActivityRequest*,
                                  grpc::ServerWriter<activity_log::ActivityFileChunk>* stream) override;

private:
    Parser _parser;
};