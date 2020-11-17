#pragma once

#include <string>

#include "activity.h"
#include "interthreadqueue.h"


namespace gpxfile
{

bool isFileDataSupported(const std::string& fileData);

bool parseFileData(const std::string& fileData, Activity& activity);

bool generateFileData(const Activity& activity, std::string& fileData);

} // namespace gpxfile
