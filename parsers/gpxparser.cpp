#include "gpxparser.h"

#include "scopeguard.h"
#include "stringutils.h"
#include "file_utils.h"

#include <cassert>
#include <iomanip>
#include <sstream>

#include "gpxfile.h"
 
bool GpxParser::canParseActivityData(const std::string& activityData) const
{
    if (!apperasToBeXmlFileData(activityData))
    {
        return false;
    }

    // Verify that it's a GPX file
    return gpxfile::isFileDataSupported(activityData);
}

bool GpxParser::parseActivityData(const std::string& activityData, Container<TrackPoint> &output) const
{
    return gpxfile::parseFileData(activityData, output);
}
