#include "cmd_plot.h"

#include <iostream>

#include "cmdutils.h"
#include "activity_log.h"


PlotCommandHandler::PlotCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int PlotCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    Activity activity;
    auto status = activityLog->getActivity(args[0], activity);
    if (!status.ok())
    {
        std::cout << "Failed to find activity " << args[0] << std::endl;
        return -1;
    }

    std::vector<activity_log::DataPoint> dataPoints;
    status = activity.plot(dataPoints);
    if (!status.ok())
    {
        std::cout << "Failed to get data points for activity " << args[0] << std::endl;
        return -1;
    }

    std::cout << "time,altitude,cummulative_distance,speed,grade,heart_rate,cummulative_ascent,cummulative_descent\n";
    for (const auto& dataPoint : dataPoints)
    {
        std::cout << dataPoint.time() <<
                    csvEntry(dataPoint.altitude()) <<
                    csvEntry(dataPoint.cummulative_distance()) <<
                    csvEntry(dataPoint.speed()) <<
                    csvEntry(dataPoint.grade()) <<
                    csvEntry(dataPoint.heart_rate()) <<
                    csvEntry(dataPoint.cummulative_ascent()) <<
                    csvEntry(dataPoint.cummulative_descent()) << std::endl;
    }

    return 0;
}

std::string PlotCommandHandler::syntax() const
{
    return name() + " <activity_id>";
}

std::string PlotCommandHandler::description() const
{
    return "plot sctivity data points to CSV format";
}

size_t PlotCommandHandler::minNumberOfArgs() const
{
    return 1;
}

size_t PlotCommandHandler::maxNumberOfArgs() const
{
    return 1;
}
