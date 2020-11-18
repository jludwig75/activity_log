#include "catch.hpp"

#include <string>
#include <thread>

#include <string.h>

#include "gpxfile.h"
#include "fileutils.h"
#include "threadfile.h"


const std::string kGpxFilePath = "../../../Big_CC_Loop.gpx";



TEST_CASE( "gpxfile", "" )
{
    SECTION( "Parse GPX file" )
    {
        std::string fileData;
        REQUIRE(readFile(kGpxFilePath, fileData));

        REQUIRE(gpxfile::isFileDataSupported(fileData));

        Activity activity;
        REQUIRE(gpxfile::parseFileData(fileData, activity));

        REQUIRE(activity.trackPoints.size() == 2113);
    }
    SECTION( "Parse, generate and reparse GPX file" )
    {
        std::string fileData;
        REQUIRE(readFile(kGpxFilePath, fileData));

        REQUIRE(gpxfile::isFileDataSupported(fileData));

        Activity activity;
        REQUIRE(gpxfile::parseFileData(fileData, activity));
        REQUIRE(activity.trackPoints.size() == 2113);

        {
            std::string outputFileData;
            REQUIRE(gpxfile::generateFileData(activity, outputFileData));
            REQUIRE(writeFile("delme.gpx", outputFileData));
        }

        std::string newFileData;
        REQUIRE(readFile("delme.gpx", newFileData));

        Activity newActivity;
        REQUIRE(gpxfile::parseFileData(newFileData, newActivity));
        REQUIRE(newActivity.trackPoints.size() == 2113);

        for (size_t i = 0; i < newActivity.trackPoints.size(); ++i)
        {
            const TrackPoint& oldTrackPoint = activity.trackPoints[i];
            const TrackPoint& newTrackPoint = newActivity.trackPoints[i];

            REQUIRE(newTrackPoint.time == oldTrackPoint.time);
            REQUIRE(newTrackPoint.latitude == oldTrackPoint.latitude);
            REQUIRE(newTrackPoint.longitude == oldTrackPoint.longitude);
            REQUIRE(newTrackPoint.altitude == oldTrackPoint.altitude);
            REQUIRE(newTrackPoint.heartRate == oldTrackPoint.heartRate);
            REQUIRE(newTrackPoint.startOfSegement == oldTrackPoint.startOfSegement);
        }
    }
}
