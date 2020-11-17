#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "activity_log.grpc.pb.h"


class Activity
{
public:
    Activity();
    Activity(activity_log::Activity& activity, std::shared_ptr<activity_log::ActivityLog::Stub> stub);
    const std::string& id() const;
    const std::string& name() const;

    std::chrono::system_clock::time_point startTime() const;
    std::chrono::system_clock::duration duration() const;
    std::chrono::system_clock::duration movingTime() const;

    float totalDistance() const;
    float totalAscent() const;
    float totalDescent() const;
    float average_speed() const;
    float moving_average_speed() const;
    float max_speed() const;
    uint32_t average_heart_rate() const;
    uint32_t max_heart_rate() const;
    float average_climbing_grade() const;
    float average_descending_grade() const;

    grpc::Status edit(const std::string& newName);
    grpc::Status del();
    grpc::Status plot(std::vector<activity_log::DataPoint>& dataPoints) const;
    grpc::Status getTracks(std::vector<activity_log::TrackPoint>& trackPoints) const;
    grpc::Status downloadToGpx(const std::string& gpxFileName) const;

private:
    activity_log::ActivityRequest activityRequest() const;
    bool _defunc;
    activity_log::Activity _activity;
    std::shared_ptr<activity_log::ActivityLog::Stub> _stub;
};