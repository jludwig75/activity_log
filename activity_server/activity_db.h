#pragma once

#include <string>
#include <vector>
#include <map>

#include "activity.h"

class Activity;


class ActivityDatabase
{
public:
    // Create
    bool storeActivity(const Activity& activity, std::string& activityId);

    // Retrieve
    bool loadActivity(const std::string& activityId, Activity& activity) const;
    using ActivityMap = std::map<std::string, Activity>;
    bool listActivities(ActivityMap& activities) const;

    // Update
    bool updateActivity(const std::string& activityId, const Activity& activity);

    // Delete
    bool deleteActivity(const std::string& activityId);

private:
    std::map<std::string, Activity> _activities;
};