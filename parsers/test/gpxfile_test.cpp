#include "catch.hpp"

#include <string>
#include <thread>

#include <string.h>

#include "gpxfile.h"
#include "threadfile.h"


const std::string kGpxFilePath = "../../../Big_CC_Loop.gpx";


const size_t kTestFileChunkSize = 64 * 1024;

bool readFile(const std::string& fileName, std::string& fileData)
{
    Container<threadfile::FileChunk> chunks;
    bool success = false;
    std::thread reader([fileName, &chunks, &success]{
        success = threadfile::readFile(fileName, kTestFileChunkSize, chunks);
    });

    threadfile::FileChunk chunk;
    while (chunks.pop(chunk))
    {
        fileData += std::string(chunk.data(), chunk.data() + chunk.size());
    }

    reader.join();

    return true;
}

bool writeFile(const std::string& fileName, const std::string& fileData)
{
    Container<threadfile::FileChunk> chunks;
    std::thread chunker([fileName, fileData, &chunks]{
        std::string::size_type offset = 0;
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
    });

    auto ret = threadfile::writeFile(fileName, chunks);
    chunker.join();
    return ret;
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
