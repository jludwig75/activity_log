#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "parser.h"

#include "threadfile.h"


int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.empty())
    {
        std::cout << "You must specify an activity file name\n";
        return -1;
    }

    Parser parser;

    auto fileName = args[0];
    Container<threadfile::FileChunk> chunks;
    bool readerSuccess = false;
    std::thread reader([fileName, &chunks, &readerSuccess]{
        readerSuccess = threadfile::readFile(fileName, 64 * 1024, chunks);
    });

    std::string activityData;
    threadfile::FileChunk chunk;
    while(chunks.pop(chunk))
    {
        activityData += std::string(chunk.data(), chunk.data() + chunk.size());
    }
    reader.join();
    if (!readerSuccess)
    {
        std::cout << "Failed to read activity file \"" << fileName << "\"\n";
        return -1;
    }

    Container<TrackPoint> output;
    int parserRet = -1;
    std::thread parserThread([&parser, &output, &activityData, &parserRet]{
        parserRet = parser.parseActivityData(activityData, output) ? 0 : -1;
    });

    std::cout << "time,latitude,logitude,altitude\n";
    TrackPoint trackPoint;
    while (output.pop(trackPoint))
    {
        std::cout << std::chrono::system_clock::to_time_t(trackPoint.time) << "," << trackPoint.latitude << "," << trackPoint.longitude << "," << trackPoint.altitude << std::endl;
    }
    parserThread.join();
    if (parserRet == -1)
    {
        std::cout << "Failed to parse \"" << fileName << "\"\n";
        return -1;
    }

    return 0;
}