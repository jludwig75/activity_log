#include "gpxparser.h"

#include "file_utils.h"
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

bool GpxParser::parseActivityData(const std::string& activityData, Activity& activity) const
{
    return gpxfile::parseFileData(activityData, activity);
}
