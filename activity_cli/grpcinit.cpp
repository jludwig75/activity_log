#include "grpcinit.h"

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>


std::shared_ptr<ActivityLog> createActivityLog()
{
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    return std::make_shared<ActivityLog>(channel);
}