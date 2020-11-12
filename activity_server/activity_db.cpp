#include "activity_db.h"


#include <iostream>
#include <ctime>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

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

ActivityDatabase::ActivityDatabase()
{
    mongocxx::instance instance{}; // This should be done only once.
    _client = std::make_unique<mongocxx::client>(mongocxx::uri{});
    mongocxx::client client{mongocxx::uri{}};
}

bool ActivityDatabase::storeActivity(const Activity& activity, std::string& activityId)
{
    activityId = gen_random(32);

    mongocxx::database db = (*_client)["activity_log"];
    mongocxx::collection collection = db["activities"];
    auto builder = bsoncxx::builder::stream::document{};

    auto inArray = builder << "id" << activityId << "name" << activity.name << "trackPoints" << bsoncxx::builder::stream::open_array;

    for (const auto& trackPoint : activity.trackPoints)
    {
        inArray = inArray << bsoncxx::builder::stream::open_document
                << "time" << std::chrono::system_clock::to_time_t(trackPoint.time)
                << "latitude" << trackPoint.latitude
                << "longitude" << trackPoint.longitude
                << "altitude" << trackPoint.altitude
                << "heartRate" << static_cast<int>(trackPoint.heartRate)
                << bsoncxx::builder::stream::close_document;
    }

    auto afterArray = inArray << close_array;
    bsoncxx::document::value doc_value = afterArray << bsoncxx::builder::stream::finalize;

    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(std::move(doc_value));

    _activities[activityId] = activity;
    return true;
}

bool ActivityDatabase::loadActivity(const std::string& activityId, Activity& activity) const
{
    mongocxx::database db = (*_client)["activity_log"];
    mongocxx::collection collection = db["activities"];

    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
    collection.find_one(document{} << "id" << activityId << finalize);
    if(maybe_result) {
        std::cout << bsoncxx::to_json(*maybe_result) << "\n";
    }

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
    return true;
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
