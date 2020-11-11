#pragma once

#include "parser.h"


class GpxParser : public ActivityParser
{
public:
    bool canParseFile(const std::string& fileName) const override;
    bool parseFile(const std::string& fileName, Container<TrackPoint> &output) const override;
};