#include "activity_db.h"


bool ActivityDatabase::storeActivity(const Activity& activity, std::string& activityId)
{
    activityId = "deg69786gg6fsg78";
    _activities[activityId] = activity;
    return true;
}

bool ActivityDatabase::loadActivity(const std::string& activityId, Activity& activity) const
{
    auto entry = _activities.find(activityId);
    if (entry == _activities.end())
    {
        return false;
    }

    activity = entry->second;
    return true;
}

bool ActivityDatabase::listActivities(ActivityMap& activities) const
{
    activities = _activities;

    return true;
}

