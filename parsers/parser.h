#pragma once

#include <memory>
#include <string>
#include <vector>

#include "container.h"
#include "activity.h"

class ActivityParser
{
public:
    virtual ~ActivityParser() {}
    virtual bool canParseActivityData(const std::string& activityData) const = 0;
    virtual bool parseActivityData(const std::string& activityData, Activity& activity) const = 0;
};


class Parser
{
public:
    Parser();
    bool parseActivityData(const std::string& activityData, Activity& activity) const;
private:
    std::vector<std::shared_ptr<ActivityParser> > _parsers;
};