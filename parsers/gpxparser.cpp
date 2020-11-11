#include "gpxparser.h"


bool GpxParser::canParseFile(const std::string& fileName) const
{
    // TODO: make case insensitive
    return fileName.length() >= 5 && fileName.substr(fileName.length() - 4) == ".gpx";
}

bool GpxParser::parseFile(const std::string& fileName, Container<TrackPoint> &output) const
{
    return false;
}
