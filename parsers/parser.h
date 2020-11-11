#pragma once

#include <memory>
#include <string>
#include <vector>

#include "conatiner.h"
#include "track_point.h"

class ActivityParser
{
public:
    virtual ~ActivityParser() {}
    virtual bool canParseActivityData(const std::string& activityData) const = 0;
    virtual bool parseActivityData(const std::string& activityData, Container<TrackPoint> &output) const = 0;
};


class Parser
{
public:
    Parser();
    bool parseActivityData(const std::string& activityData, Container<TrackPoint> &output) const;
private:
    std::vector<std::shared_ptr<ActivityParser> > _parsers;
};