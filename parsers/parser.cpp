#include "parser.h"

#include "gpxparser.h"


Parser::Parser()
    :
    _parsers{ std::make_shared<GpxParser>() }
{
}

bool Parser::parseActivityData(const std::string& activityData, Container<TrackPoint> &output) const
{
    for (const auto& parser : _parsers)
    {
        if (parser->canParseActivityData(activityData))
        {
            return parser->parseActivityData(activityData, output);
        }
    }

    return false;
}
