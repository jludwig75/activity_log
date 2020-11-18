#include "cmd_track.h"

#include "cmdutils.h"
#include "activity_log.h"


TrackCommandHandler::TrackCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int TrackCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    Activity activity;
    auto status = activityLog->getActivity(args[0], activity);
    if (!status.ok())
    {
        std::cout << "Failed to find activity " << args[0] << std::endl;
        return -1;
    }

    std::vector<activity_log::TrackPoint> trackPoints;
    status = activity.getTracks(trackPoints);
    if (!status.ok())
    {
        std::cout << "Failed to get track points for activity " << args[0] << std::endl;
        return -1;
    }

    std::cout << "time,latitude,longitude,altitude,start_of_sgement\n";
    for (const auto& trackPoint : trackPoints)
    {
        std::cout << trackPoint.time() <<
                    csvEntry(trackPoint.latitude()) <<
                    csvEntry(trackPoint.longitude()) <<
                    csvEntry(trackPoint.altitude()) <<
                    csvEntry(trackPoint.start_of_sgement()) << std::endl;
    }

    return 0;
}

std::string TrackCommandHandler::syntax() const
{
    return name() + " <activity_id>";
}

std::string TrackCommandHandler::description() const
{
    return "retreive activity track points in CSV format";
}

size_t TrackCommandHandler::minNumberOfArgs() const
{
    return 1;
}

size_t TrackCommandHandler::maxNumberOfArgs() const
{
    return 1;
}

