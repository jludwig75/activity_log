#include "catch.hpp"

#include <thread>

#include "../activity_db.h"
#include "gpxfile.h"
#include "parser.h"
#include "file_utils.h"
#include "threadfile.h"


const size_t kTestFileChunkSize = 64 * 1024;

bool readFile(const std::string& fileName, std::string& fileData)
{
    InterThreadQueue<threadfile::FileChunk> chunks;
    bool readerSuccess = false;
    std::thread reader([fileName, &chunks, &readerSuccess]{
        readerSuccess = threadfile::readFile(fileName, kTestFileChunkSize, chunks);
    });

    threadfile::FileChunk chunk;
    while (chunks.pop(chunk))
    {
        fileData += std::string(chunk.data(), chunk.data() + chunk.size());
    }

    reader.join();
    if (!readerSuccess)
    {
        return false;
    }

    return true;
}

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
