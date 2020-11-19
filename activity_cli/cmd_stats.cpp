#include "cmd_stats.h"

#include "activity_log.h"
#include "datetime.h"
#include "units.h"


StatsCommandHandler::StatsCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int StatsCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    activity_log::Stats stats;
    if (!activityLog->calculateStats(std::chrono::system_clock::from_time_t(0), std::chrono::system_clock::now(), stats).ok())
    {
        std::cout << "Failed to calculate stats\n";
        return -1;
    }

    std::cout << "Cummulative activtiy stats:\n";
    std::cout << "\tNumber of activities: " << stats.total_activities() << std::endl;
    std::cout << "\tTime spent: " << toDuration(std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds(stats.total_time()))) << std::endl;
    std::cout << "\tTotal distance: " << m_to_miles(stats.total_distance()) << " miles" << std::endl;
    std::cout << "\tTotla climbing: " << m_to_ft(stats.total_ascent()) << " feet" << std::endl;

    return 0;
}

std::string StatsCommandHandler::syntax() const
{
    return name();
}

std::string StatsCommandHandler::description() const
{
    return "calculate cummulaive activity statistics";
}

