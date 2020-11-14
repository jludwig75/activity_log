#include "catch.hpp"

#include <string>

#include <string.h>

#include "gpxfile.h"
#include "threadfile.h"


const std::string kGpxFilePath = "../../../Big_CC_Loop.gpx";


const size_t kTestFileChunkSize = 64 * 1024;

bool readFile(const std::string& fileName, std::string& fileData)
{
    Container<threadfile::FileChunk> chunks;
    if (!threadfile::readFile(fileName, kTestFileChunkSize, chunks))
    {
        return false;
    }

    for (const auto& chunk: chunks)
    {
        fileData += std::string(chunk.data(), chunk.data() + chunk.size());
    }

    return true;
}

bool writeFile(const std::string& fileName, const std::string& fileData)
{
    std::string::size_type offset = 0;
    Container<threadfile::FileChunk> chunks;
    while (offset < fileData.length())
    {
        threadfile::FileChunk chunk(kTestFileChunkSize);
        auto chunkString = fileData.substr(offset, chunk.maxSize());
        memcpy(chunk.data(), chunkString.data(), chunkString.length());
        chunk.setSize(chunkString.length());
        offset += chunkString.length();

        chunks.push(std::move(chunk));
    }
    chunks.done_pushing();

    return threadfile::writeFile(fileName, chunks);
}

TEST_CASE( "gpxfile", "" )
{
    SECTION( "Parse GPX file" )
    {
        std::string fileData;
        REQUIRE(readFile(kGpxFilePath, fileData));

        REQUIRE(gpxfile::isFileDataSupported(fileData));

        Container<TrackPoint> trackPoints;
        REQUIRE(gpxfile::parseFileData(fileData, trackPoints));

        REQUIRE(trackPoints.size() == 2113);
    }
    SECTION( "Parse, generate and reparse GPX file" )
    {
        std::string fileData;
        REQUIRE(readFile(kGpxFilePath, fileData));

        REQUIRE(gpxfile::isFileDataSupported(fileData));

        Container<TrackPoint> trackPoints;
        REQUIRE(gpxfile::parseFileData(fileData, trackPoints));
        REQUIRE(trackPoints.size() == 2113);

        {
            std::string outputFileData;
            REQUIRE(gpxfile::generateFileData(trackPoints, outputFileData));
            REQUIRE(writeFile("delme.gpx", outputFileData));
        }

        std::string newFileData;
        REQUIRE(readFile("delme.gpx", newFileData));

        Container<TrackPoint> newTrackPoints;
        REQUIRE(gpxfile::parseFileData(newFileData, newTrackPoints));
        REQUIRE(newTrackPoints.size() == 2113);

        for (size_t i = 0; i < newTrackPoints.size(); ++i)
        {
            const TrackPoint& oldTrackPoint = trackPoints[i];
            const TrackPoint& newTrackPoint = newTrackPoints[i];

            REQUIRE(newTrackPoint.time == oldTrackPoint.time);
            REQUIRE(newTrackPoint.latitude == oldTrackPoint.latitude);
            REQUIRE(newTrackPoint.longitude == oldTrackPoint.longitude);
            REQUIRE(newTrackPoint.altitude == oldTrackPoint.altitude);
            REQUIRE(newTrackPoint.heartRate == oldTrackPoint.heartRate);
            REQUIRE(newTrackPoint.startOfSegement == oldTrackPoint.startOfSegement);
            REQUIRE(newTrackPoint.activityName == oldTrackPoint.activityName);
        }
    }
}
