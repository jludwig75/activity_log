#pragma once

#include <string>
#include <vector>
#include <map>

#include "activity.h"

class Activity;


class ActivityDatabase
{
public:
    bool storeActivity(const Activity& activity, std::string& activityId);
    bool loadActivity(const std::string& activityId, Activity& activity) const;
    
    using ActivityMap = std::map<std::string, Activity>;
    bool listActivities(ActivityMap& activities) const;
private:
    std::map<std::string, Activity> _activities;
};