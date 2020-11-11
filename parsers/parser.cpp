#include "parser.h"

#include "gpxparser.h"


Parser::Parser()
    :
    _parsers{ std::make_shared<GpxParser>() }
{
}

bool Parser::parseFile(const std::string& fileName, Container<TrackPoint> &output) const
{
    for (const auto& parser : _parsers)
    {
        if (parser->canParseFile(fileName))
        {
            return parser->parseFile(fileName, output);
        }
    }

    return false;
}
