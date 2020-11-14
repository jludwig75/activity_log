#include "parser.h"

#include "gpxparser.h"


Parser::Parser()
    :
    _parsers{ std::make_shared<GpxParser>() }
{
}

bool Parser::parseActivityData(const std::string& activityData, Activity& activity) const
{
    for (const auto& parser : _parsers)
    {
        if (parser->canParseActivityData(activityData))
        {
            return parser->parseActivityData(activityData, activity);
        }
    }

    return false;
}
