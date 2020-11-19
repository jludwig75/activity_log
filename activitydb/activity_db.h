#pragma once

#include <string>
#include <memory>
#include <vector>

#include "activity.h"

class Activity;

class DbWrapper;

class ActivityDatabase
{
public:
    ActivityDatabase();
    // Create
    bool storeActivity(const Activity& activity, std::string& activityId);

    // Retrieve
    bool loadActivity(const std::string& activityId, Activity& activity) const;

    using ActivityList = std::vector<Activity>;
    bool listActivities(ActivityList& activities, bool includeTrackPoints = true) const;

    // Update
    bool updateActivity(const std::string& activityId, const Activity& activity);

    // Delete
    bool deleteActivity(const std::string& activityId);

private:
    std::shared_ptr<DbWrapper> _wrapper;
};