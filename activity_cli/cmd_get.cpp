#include "cmd_get.h"

#include <boost/format.hpp>

#include "activity_log.h"
#include "datetime.h"
#include "units.h"


GetCommandHandler::GetCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int GetCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    Activity activity;
    auto status = activityLog->getActivity(args[0], activity);
    if (!status.ok())
    {
        std::cout << "Failed to find activity " << args[0] << std::endl;
        return -1;
    }

    std::cout << "Activity " << activity.id() << std::endl;
    std::cout << boost::format("\tname: %1%\n") % activity.name();
    std::cout << boost::format("\tstartTime: %1%\n") % toTimeString(activity.startTime());
    std::cout << boost::format("\tduration: %1%\n") % toDuration(activity.duration());
    std::cout << boost::format("\tmovingTime: %1%\n") % toDuration(activity.movingTime());
    std::cout << boost::format("\ttotalDistance: %1% miles\n") % m_to_miles(activity.totalDistance());
    std::cout << boost::format("\ttotalAscent: %1%\n") %m_to_ft(activity.totalAscent());
    std::cout << boost::format("\ttotalDescent: %1% feet\n") % m_to_ft(activity.totalDescent());
    std::cout << boost::format("\taverageSpeed: %1% mph\n") % mps_to_mph(activity.average_speed());
    std::cout << boost::format("\tmovingAverageSpeed: %1% mph\n") % mps_to_mph(activity.moving_average_speed());
    std::cout << boost::format("\tmaxSpeed: %1% mph\n") % mps_to_mph(activity.max_speed());
    std::cout << boost::format("\taverageHeartRate: %1% bpm\n") % activity.average_heart_rate();
    std::cout << boost::format("\tmaxHeartRate: %1% bpm\n") % activity.max_heart_rate();
    std::cout << boost::format("\taverageClimbingGrade: %1%%%\n") % (100.0 * activity.average_climbing_grade());
    std::cout << boost::format("\taverageDescendingGrade: %1%%%\n") % (100.0 * activity.average_descending_grade());

    return 0;
}

std::string GetCommandHandler::syntax() const
{
    return name() + " <activity_id>";
}

std::string GetCommandHandler::description() const
{
    return "retrieves activity details";
}

int GetCommandHandler::minNumberOfArgs() const
{
    return 1;
}

int GetCommandHandler::maxNumberOfArgs() const
{
    return 1;
}
