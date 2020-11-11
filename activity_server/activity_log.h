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
private:
    Parser _parser;
};