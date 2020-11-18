#include "cmd_get.h"

#include "activity_log.h"
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
    std::cout << "\tname: " << activity.name() << std::endl;
    // TODO: Make human readable
    std::cout << "\tstartTime: " << std::chrono::system_clock::to_time_t(activity.startTime()) << std::endl;
    // TODO: Show as HMS
    std::cout << "\tduration: " << std::chrono::duration_cast<std::chrono::seconds>(activity.duration()).count() << std::endl;
    std::cout << "\tmovingTime: " << std::chrono::duration_cast<std::chrono::seconds>(activity.movingTime()).count() << std::endl;
    std::cout << "\ttotalDistance: " << m_to_miles(activity.totalDistance()) << " miles" << std::endl;
    std::cout << "\ttotalAscent: " << m_to_ft(activity.totalAscent()) << " feet" << std::endl;
    std::cout << "\ttotalDescent: " << m_to_ft(activity.totalDescent()) << " feet" << std::endl;
    std::cout << "\taverageSpeed: " << mps_to_mph(activity.average_speed()) << " mph" << std::endl;
    std::cout << "\tmovingAverageSpeed: " << mps_to_mph(activity.moving_average_speed()) << " mph" << std::endl;
    std::cout << "\tmaxSpeed: " << mps_to_mph(activity.max_speed()) << " mph" << std::endl;
    std::cout << "\taverageHeartRate: " << activity.average_heart_rate() << " bpm" << std::endl;
    std::cout << "\tmaxHeartRate: " << activity.max_heart_rate() << " bpm" << std::endl;
    std::cout << "\taverageClimbingGrade: " << 100.0 * activity.average_climbing_grade() << "%" << std::endl;
    std::cout << "\taverageDescendingGrade: " << 100.0 * activity.average_descending_grade() << "%" << std::endl;

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
size_t GetCommandHandler::minNumberOfArgs() const
{
    return 1;
}
size_t GetCommandHandler::maxNumberOfArgs() const
{
    return 1;
}
