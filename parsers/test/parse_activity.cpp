#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "parser.h"


int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.empty())
    {
        std::cout << "You must specify an activity file name\n";
        return -1;
    }

    Parser parser;

    Container<TrackPoint> output;
    int ret = 0;
    auto fileName = args[0];
    std::thread parserThread([&parser, &output, fileName, &ret]{
        if (!parser.parseFile(fileName, output))
        {
            ret = -1;
        }
    });

    TrackPoint trackPoint;
    while (output.pop(trackPoint))
    {
        std::cout << std::chrono::system_clock::to_time_t(trackPoint.time) << std::endl;
    }

    parserThread.join();
    if (ret == -1)
    {
        std::cout << "Failed to parse \"" << fileName << "\"\n";
        return -1;
    }

    return 0;
}