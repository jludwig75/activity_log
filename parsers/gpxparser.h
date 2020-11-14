#pragma once

#include "parser.h"


class GpxParser : public ActivityParser
{
public:
    bool canParseActivityData(const std::string& activityData) const override;
    bool parseActivityData(const std::string& activityData, Activity& activity) const override;
};