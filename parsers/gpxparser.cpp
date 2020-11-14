#include "gpxparser.h"

#include "file_utils.h"

#include <cassert>
#include <functional>
#include <iomanip>
#include <sstream>

#include "pugixml.hpp"
 
namespace pugiparser
{

bool isGpxFile(const std::string& gpxFileData)
{
    pugi::xml_document doc;
    auto result = doc.load_string(gpxFileData.c_str());
    if (!result)
    {
        return false;
    }

    return doc.child("gpx");
}

template<typename T>
bool fromString(const std::string& str, T& value)
{
    std::stringstream ss(str);

    ss >> value;
    return !ss.bad() && ss.eof();
}

bool fromTimeString(const std::string& timeStr, std::time_t& time)
{
    std::tm tm = {};
    std::stringstream ss(timeStr);
    // 2020-10-31T16:35:15Z
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
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

class OnExit
{
public:
    using ExitFunc = std::function<void(void)>;
    OnExit(ExitFunc&& exitFunc)
        :
        _exitFunc(exitFunc)
    {

    }
    ~OnExit()
    {
        _exitFunc();
    }
private:
    ExitFunc _exitFunc;
};
#define ON_EXIT(func)   OnExit onExitBloc_##__FILE__##__LINE__(func)

bool parseFile(const std::string& gpxFileData, Container<TrackPoint>& stream)
{
    ON_EXIT([&stream]{ stream.done_pushing(); });
    pugi::xml_document doc;
    auto result = doc.load_string(gpxFileData.c_str());
    if (!result)
    {
        return false;
    }

    auto gpxElement = doc.child("gpx");
    if (!gpxElement)
    {
        return false;
    }

    std::string activityName;
    for (pugi::xml_node trkElement: gpxElement.children("trk"))
    {
        if (activityName.empty())
        {
            auto nameElement = trkElement.child("name");
            if (nameElement)
            {
                activityName = nameElement.child_value();
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
                if (!fromString(eleElement.child_value(), trackPoint.altitude))
                {
                    return false;
                }
                std::time_t time;
                if (!fromTimeString(timeElement.child_value(), time))
                {
                    return false;
                }
                trackPoint.time = std::chrono::system_clock::from_time_t(time);

                trackPoint.startOfSegement = startOfSegment;

                if (stream.empty() && !activityName.empty())
                {
                    trackPoint.activityName = activityName;
                }

                startOfSegment = false;
                stream.push(std::move(trackPoint));
            }
        }
    }

    return true;
}

}


bool GpxParser::canParseActivityData(const std::string& activityData) const
{
    if (!apperasToBeXmlFileData(activityData))
    {
        return false;
    }

    // Verify that it's a GPX file
    return pugiparser::isGpxFile(activityData);
}

bool GpxParser::parseActivityData(const std::string& activityData, Container<TrackPoint> &output) const
{
    return pugiparser::parseFile(activityData, output);
}
