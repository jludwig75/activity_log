#include "activity_log.h"


using grpc::Status;


grpc::Status ActivityLog::uploadActivity(grpc::ServerContext* context,
                            grpc::ServerReader<activity_log::ActivityFileChunk>* stream,
                            activity_log::Activity* activity)
{
    return Status::OK;
}
