#include "activity_db.h"

#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
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

bool desrializeActivity(bsoncxx::document::view& activityView, Activity& activity)
{
    auto id = activityView["_id"].get_oid().value.to_string();
    auto name = activityView["name"].get_utf8().value.to_string();
    activity.start_time = std::chrono::system_clock::from_time_t(activityView["startTime"].get_int64().value);

    activity.id = id;
    activity.name = name;

    auto trackPoints = activityView["trackPoints"].get_array().value;
    for (const auto& trackPointElement : trackPoints)
    {
        TrackPoint trackPoint;
        bsoncxx::document::view trackPointView = trackPointElement.get_document().value;
        trackPoint.time = std::chrono::system_clock::from_time_t(trackPointView["time"].get_int64().value);
        trackPoint.latitude = trackPointView["latitude"].get_double().value;
        trackPoint.longitude = trackPointView["longitude"].get_double().value;
        trackPoint.altitude = trackPointView["altitude"].get_double().value;
        trackPoint.heartRate = trackPointView["heartRate"].get_int32().value;
        trackPoint.startOfSegement = trackPointView["startOfSegement"].get_bool().value;
        activity.trackPoints.push_back(std::move(trackPoint));
    }

    activity.analyzeTrackPoints();

    return true;
}

bsoncxx::document::value serializeActivity(const Activity& activity)
{
    auto builder = bsoncxx::builder::stream::document{};

    auto inArray = builder << "name" << activity.name
                    << "startTime" << std::chrono::system_clock::to_time_t(activity.start_time)
                    << "trackPoints" << bsoncxx::builder::stream::open_array;

    for (const auto& trackPoint : activity.trackPoints)
    {
        inArray = inArray << bsoncxx::builder::stream::open_document
                << "time" << std::chrono::system_clock::to_time_t(trackPoint.time)
                << "latitude" << trackPoint.latitude
                << "longitude" << trackPoint.longitude
                << "altitude" << trackPoint.altitude
                << "heartRate" << static_cast<int>(trackPoint.heartRate)
                << "startOfSegement" << trackPoint.startOfSegement
                << bsoncxx::builder::stream::close_document;
    }

    auto afterArray = inArray << close_array;
    return afterArray << bsoncxx::builder::stream::finalize;
}

bsoncxx::document::value makeActivityId(const std::string& activityId)
{
    auto builder = bsoncxx::builder::stream::document{};
    return builder << "_id"
                        << bsoncxx::oid{mongocxx::stdx::string_view{activityId}}
                        << bsoncxx::builder::stream::finalize;
}


}

class DbWrapper
{
public:
    DbWrapper()
        :
        client(mongocxx::uri{})
    {
    }
    mongocxx::v_noabi::client client;
};

ActivityDatabase::ActivityDatabase()
{
    mongocxx::instance instance{}; // This should be done only once.
    _wrapper = std::make_shared<DbWrapper>();
}

bool ActivityDatabase::storeActivity(const Activity& activity, std::string& activityId)
{
    mongocxx::database db = _wrapper->client["activity_log"];

    auto result = db["activities"].insert_one(serializeActivity(activity));
    if (!result)
    {
        return false;
    }

    activityId = result->inserted_id().get_oid().value.to_string();

    return true;
}

bool ActivityDatabase::loadActivity(const std::string& activityId, Activity& activity) const
{
    mongocxx::database db = _wrapper->client["activity_log"];

    auto result = db["activities"].find_one(makeActivityId(activityId));
    if(!result)
    {
        return false;
    }

    auto view = result->view();
    if (!desrializeActivity(view, activity))
    {
        return false;
    }

    return true;
}

bool ActivityDatabase::listActivities(ActivityMap& activities) const
{
    mongocxx::database db = _wrapper->client["activity_log"];
    
    auto cursor = db["activities"].find({});
    std::vector<bsoncxx::document::view> activitiesView{cursor.begin(), cursor.end()};
    for (std::size_t i = 0; i < activitiesView.size(); ++i)
    {
        auto activityView = activitiesView[i];
        Activity activity;
        if (!desrializeActivity(activityView, activity))
        {
            return false;
        }
        activities[activity.id] = activity;
    }

    return true;
}

bool ActivityDatabase::updateActivity(const std::string& activityId, const Activity& activity)
{
    mongocxx::database db = _wrapper->client["activity_log"];

    db["activities"].replace_one(makeActivityId(activityId), serializeActivity(activity));

    return true;
}

bool ActivityDatabase::deleteActivity(const std::string& activityId)
{
    mongocxx::database db = _wrapper->client["activity_log"];

    db["activities"].delete_one(makeActivityId(activityId));

    return true;
}
