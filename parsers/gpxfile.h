#pragma once

#include <string>

#include "track_point.h"
#include "container.h"


namespace gpxfile
{

bool isSupportedFileData(const std::string& fileData);

bool parseFileData(const std::string& fileData, Container<TrackPoint>& stream);

bool generateFileData(const Container<TrackPoint>& stream, std::string& fileData);

} // namespace gpxfile
