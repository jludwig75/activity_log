#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "parser.h"


namespace
{

std::string readFile(const std::string& fileName)
{
    std::ifstream t(fileName);
    std::string str;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    return str;
}

}


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
    auto activityData = readFile(fileName);

    Container<TrackPoint> output;
    int ret = 0;
    std::thread parserThread([&parser, &output, &activityData, &ret]{
        if (!parser.parseActivityData(activityData, output))
        {
            ret = -1;
        }
    });

    std::cout << "time,latitude,logitude,altitude\n";
    TrackPoint trackPoint;
    while (output.pop(trackPoint))
    {
        std::cout << std::chrono::system_clock::to_time_t(trackPoint.time) << "," << trackPoint.latitude << "," << trackPoint.longitude << "," << trackPoint.altitude << std::endl;
    }

    parserThread.join();
    if (ret == -1)
    {
        std::cout << "Failed to parse \"" << fileName << "\"\n";
        return -1;
    }

    return 0;
}