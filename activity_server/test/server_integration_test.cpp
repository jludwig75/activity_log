#include "catch.hpp"

#include <thread>

#include "activity_db.h"
#include "gpxfile.h"
#include "parser.h"
#include "file_utils.h"
#include "fileutils.h"
#include "threadfile.h"


Parser parser;
ActivityDatabase db;

TEST_CASE("parser database integration test", "")
{
    SECTION("GPX with heart rate: parse, store, retrieve, generate ")
    {
        std::string fileData;
        REQUIRE(readFile("../../../Finding_My_Inner_Mountain_Goat.gpx", fileData));

        Activity parsedActivity;
        REQUIRE(parser.parseActivityData(fileData, parsedActivity));

        std::string activityId;
        REQUIRE(db.storeActivity(parsedActivity, activityId));

        Activity retrievedActivity;
        REQUIRE(db.loadActivity(activityId, retrievedActivity));

        REQUIRE((parsedActivity == retrievedActivity));

        std::string generatedFileData;
        REQUIRE(gpxfile::generateFileData(retrievedActivity, generatedFileData));

        REQUIRE(generatedFileData == fileData);
    }

    SECTION("GPX without heart rate: parse, store, retrieve, generate ")
    {
        std::string fileData;
        REQUIRE(readFile("../../../Big_CC_Loop.gpx", fileData));

        Activity parsedActivity;
        REQUIRE(parser.parseActivityData(fileData, parsedActivity));

        std::string activityId;
        REQUIRE(db.storeActivity(parsedActivity, activityId));

        Activity retrievedActivity;
        REQUIRE(db.loadActivity(activityId, retrievedActivity));

        REQUIRE((parsedActivity == retrievedActivity));

        std::string generatedFileData;
        REQUIRE(gpxfile::generateFileData(retrievedActivity, generatedFileData));

        REQUIRE(generatedFileData == fileData);
    }
}
