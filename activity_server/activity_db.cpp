#include "activity_db.h"


#include <iostream>
#include <ctime>
#include <unistd.h>

using namespace std;

namespace
{

string gen_random(const int len) {
    
    string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    srand( (unsigned) time(NULL) * getpid());
    
    for (int i = 0; i < len; ++i) 
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    
    
    return tmp_s;
    
}

}

bool ActivityDatabase::storeActivity(const Activity& activity, std::string& activityId)
{
    activityId = gen_random(32);
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

bool ActivityDatabase::updateActivity(const std::string& activityId, const Activity& activity)
{
    auto entry = _activities.find(activityId);
    if (entry == _activities.end())
    {
        return false;
    }

    _activities[activityId] = activity;
}

bool ActivityDatabase::deleteActivity(const std::string& activityId)
{
    auto entry = _activities.find(activityId);
    if (entry == _activities.end())
    {
        return false;
    }

    _activities.erase(entry);
    return true;
}
