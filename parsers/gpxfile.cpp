#include "gpxfile.h"

#include <iomanip>

#include "pugixml.hpp"
#include "scopeguard.h"
#include "stringutils.h"
#include "file_utils.h"


namespace gpxfile
{

namespace internal
{

const char* kGpxTimeFomratString = "%Y-%m-%dT%H:%M:%SZ";

bool fromTimeString(const std::string& timeStr, std::time_t& time)
{
    std::tm tm = {};
    std::stringstream ss(timeStr);
    // 2020-10-31T16:35:15Z
    ss >> std::get_time(&tm, kGpxTimeFomratString);
    if (ss.bad())// || !ss.eof())   // std::get_time seems to leave something in the stream!!!
    {
        return false;
    }
    // std::get_time seems to leave something in the stream!!!, but we only read the empty string.
    std::string remainder;
    ss >> remainder;
    if (!remainder.empty())
    {
        return false;
    }
    assert(ss.eof());
    time = timegm(&tm);
    return true;
}

std::string toTimeString(std::time_t time)
{
    std::tm tm;
    gmtime_r(&time, &tm);

    std::stringstream ss;
    ss << std::put_time(&tm, kGpxTimeFomratString);
    return ss.str();
}

}


bool isFileDataSupported(const std::string& fileData)
{
    pugi::xml_document doc;
    auto result = doc.load_string(fileData.c_str());
    if (!result)
    {
        return false;
    }

    return doc.child("gpx");
}

bool parseFileData(const std::string& fileData, Activity& activity)
{
    auto& stream = activity.trackPoints;
    pugi::xml_document doc;
    auto result = doc.load_string(fileData.c_str());
    if (!result)
    {
        return false;
    }

    auto gpxElement = doc.child("gpx");
    if (!gpxElement)
    {
        return false;
    }

    {
        auto metadataElement = gpxElement.child("metadata");
        if (!metadataElement)
        {
            return false;
        }

        auto timeElelment = metadataElement.child("time");
        if (!timeElelment)
        {
            return false;
        }

        auto timeStr = timeElelment.text().as_string();
        std::time_t time;
        if (!internal::fromTimeString(timeStr, time))
        {
            return false;
        }
        activity.start_time = std::chrono::system_clock::from_time_t(time);
    }

    for (pugi::xml_node trkElement: gpxElement.children("trk"))
    {
        {
            auto nameElement = trkElement.child("name");
            if (nameElement)
            {
                activity.name = nameElement.text().as_string();
            }
        }

        for (pugi::xml_node trksegElement: trkElement.children("trkseg"))
        {
            bool startOfSegment = true;
            for (pugi::xml_node trkptElement: trksegElement.children("trkpt"))
            {
                auto lat = trkptElement.attribute("lat");
                if (!lat)
                {
                    return false;
                }

                auto lon = trkptElement.attribute("lon");
                if (!lon)
                {
                    return false;
                }

                auto eleElement = trkptElement.child("ele");
                if (!eleElement)
                {
                    return false;
                }

                auto timeElement = trkptElement.child("time");
                if (!timeElement)
                {
                    return false;
                }

                TrackPoint trackPoint;
                if (!fromString(lat.value(), trackPoint.latitude))
                {
                    return false;
                }
                if (!fromString(lon.value(), trackPoint.longitude))
                {
                    return false;
                }
                trackPoint.altitude = eleElement.text().as_double();
                std::time_t time;
                if (!internal::fromTimeString(timeElement.text().as_string(), time))
                {
                    return false;
                }
                trackPoint.time = std::chrono::system_clock::from_time_t(time);

                trackPoint.startOfSegement = startOfSegment;

                startOfSegment = false;

                trackPoint.heartRate = 0;
                auto extensionsElement = trkptElement.child("extensions");
                if (extensionsElement)
                {
                    for (const auto& childElement: extensionsElement.children())
                    {
                        if (endsWith(childElement.name(), "TrackPointExtension"))
                        {
                            for (const auto& grandChildElement: childElement.children())
                            {
                                if (endsWith(grandChildElement.name(), "hr"))
                                {
                                    trackPoint.heartRate = grandChildElement.text().as_uint();
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }

                stream.push_back(std::move(trackPoint));
            }
        }
    }

    activity.analyzeTrackPoints();

    return true;
}

bool generateFileData(const Activity& activity, std::string& fileData)
{
    auto& stream = activity.trackPoints;
    if (stream.empty())
    {
        return false;
    }

    // 99.99..% of the time the first track point will have a non-zero heart
    // rate, if the activity has heart rate data. When it doesn't, this loop
    // runs longer. TODO: Maybe only look at the first few, the middle and the
    // end until we determine there is no heart rate data?
    bool hasHeartRateData = false;
    for (const auto& trackPoint: stream)
    {
        if (trackPoint.heartRate != 0)
        {
            hasHeartRateData = true;
            break;
        }
    }

    pugi::xml_document doc;

    pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";

    auto gpxElement = doc.append_child();
    gpxElement.set_name("gpx");

    // xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    gpxElement.append_attribute("creator").set_value("ActivityLogGpx");
    // xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    gpxElement.append_attribute("xmlns:xsi").set_value("http://www.w3.org/2001/XMLSchema-instance");
    // xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd"
    std::string schemaLocation = "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd";
    if (hasHeartRateData)
    {
        schemaLocation += " http://www.garmin.com/xmlschemas/GpxExtensions/v3 http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd http://www.garmin.com/xmlschemas/TrackPointExtension/v1 http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd";
    }
    gpxElement.append_attribute("xsi:schemaLocation").set_value(schemaLocation.c_str());
    // version="1.1"
    gpxElement.append_attribute("version").set_value("1.1");
    // xmlns="http://www.topografix.com/GPX/1/1"
    gpxElement.append_attribute("xmlns").set_value("http://www.topografix.com/GPX/1/1");

    if (hasHeartRateData)
    {
        // xmlns:gpxtpx="http://www.garmin.com/xmlschemas/TrackPointExtension/v1"
        gpxElement.append_attribute("xmlns:gpxtpx").set_value("http://www.garmin.com/xmlschemas/TrackPointExtension/v1");
        // xmlns:gpxx="http://www.garmin.com/xmlschemas/GpxExtensions/v3"        
        gpxElement.append_attribute("xmlns:gpxx").set_value("http://www.garmin.com/xmlschemas/GpxExtensions/v3");
    }

    {
        auto metadataElement = gpxElement.append_child();
        metadataElement.set_name("metadata");
        auto timeElement = metadataElement.append_child();
        timeElement.set_name("time");
        timeElement.text().set(internal::toTimeString(std::chrono::system_clock::to_time_t(activity.start_time)).c_str());
    }

    auto trkEelment = gpxElement.append_child();
    trkEelment.set_name("trk");

    {
        auto nameElement = trkEelment.append_child();
        nameElement.set_name("name");
        nameElement.text().set(activity.name.c_str());
    }

    {
        auto typeElement = trkEelment.append_child();
        typeElement.set_name("type");
        typeElement.text().set(std::to_string(1).c_str());
    }

    pugi::xml_node currentTrksegElement;
    for (const auto& trackPoint : stream)
    {
        if (!currentTrksegElement || trackPoint.startOfSegement)
        {
            currentTrksegElement = trkEelment.append_child();
            currentTrksegElement.set_name("trkseg");
        }

        auto trkptElement = currentTrksegElement.append_child();
        trkptElement.set_name("trkpt");

        trkptElement.append_attribute("lat").set_value(floatToString(trackPoint.latitude, 7).c_str());
        trkptElement.append_attribute("lon").set_value(floatToString(trackPoint.longitude, 7).c_str());
        trkptElement.append_child("ele").text().set(floatToString(trackPoint.altitude, 1).c_str());
        trkptElement.append_child("time").text().set(internal::toTimeString(std::chrono::system_clock::to_time_t(trackPoint.time)).c_str());

        if (hasHeartRateData)
        {
            trkptElement.append_child("extensions").append_child("gpxtpx:TrackPointExtension").append_child("gpxtpx:hr").text().set(std::to_string(trackPoint.heartRate).c_str());
        }
    }

    std::stringstream ss;
    doc.save(ss, " ");
    fileData = ss.str();

    return true;
}

} // namespace gpxfile
