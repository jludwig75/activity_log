#pragma once

#include <memory>
#include <string>
#include <vector>

#include "conatiner.h"
#include "track_point.h"

class ActivityParser
{
public:
    virtual ~ActivityParser() {}
    virtual bool canParseFile(const std::string& fileName) const = 0;
    virtual bool parseFile(const std::string& fileName, Container<TrackPoint> &output) const = 0;
};


class Parser
{
public:
    Parser();
    bool parseFile(const std::string& fileName, Container<TrackPoint> &output) const;
private:
    std::vector<std::shared_ptr<ActivityParser> > _parsers;
};