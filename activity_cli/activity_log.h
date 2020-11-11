#pragma once

#include "activity.h"

#include <chrono>
#include <memory>
#include <vector>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>

#include "activity_log.grpc.pb.h"


using system_clock = std::chrono::system_clock;


class ActivityLog
{
public:
    ActivityLog(std::shared_ptr<grpc::Channel> channel);
    grpc::Status uploadActivity(const std::string& activityFileName, Activity& newActivity);
    grpc::Status listActivities(system_clock::time_point startTime, system_clock::time_point endTime, std::vector<Activity> &activities) const;
    grpc::Status calculateStats(system_clock::time_point startTime, system_clock::time_point endTime, activity_log::Stats& stats) const;

private:
    std::shared_ptr<activity_log::ActivityLog::Stub> _stub;
};