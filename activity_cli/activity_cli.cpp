#include "activity_log.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>


template<typename ValueType>
ValueType mps_to_kph(ValueType mps)
{
    return mps * 60.0 * 60.0 / 1000.0;
}

template<typename ValueType>
ValueType mps_to_mph(ValueType mps)
{
    return mps * 60.0 * 60.0 * 3.28084 / 5280.0;
}

template<typename ValueType>
ValueType m_to_miles(ValueType m)
{
    return m * 3.28084 / 5280.0;
}

template<typename ValueType>
ValueType m_to_km(ValueType m)
{
    return m / 1000.0;
}

template<typename ValueType>
ValueType m_to_ft(ValueType m)
{
    return m * 3.28084;
}


class CommandHandler
{
public:
    CommandHandler(const std::string& name)
        :
        _name(name)
    {
    }
    virtual ~CommandHandler() {}
    virtual int runCommand(const std::vector<std::string>& args) = 0;
    virtual std::string syntax() const = 0;
    virtual std::string description() const = 0;
    virtual size_t minNumberOfArgs() const
    {
        return 0;
    }
    virtual size_t maxNumberOfArgs() const
    {
        return 0;
    }
protected:
    const std::string& name() const
    {
        return _name;
    }
private:
    const std::string _name;
};

class UploadCommandHandler : public CommandHandler
{
public:
    UploadCommandHandler(const std::string& name)
        :
        CommandHandler(name)
    {
    }

    virtual int runCommand(const std::vector<std::string>& args)
    {
        auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        ActivityLog activityLog(channel);

        Activity newActivity;
        if (!activityLog.uploadActivity(args[0], newActivity).ok())
        {
            std::cout << "Failed to upload activity \"" << args[0] << "\"\n";
            return -1;
        }

        std::cout << "Successfully uploaded activity file \"" << args[0] << "\" as activity " << newActivity.id() + "\n";

        return 0;
    }
    virtual std::string syntax() const
    {
        return name() + " <activity_file_name>";
    }
    virtual std::string description() const
    {
        return "uploads an activity file (currently only supports gpx)";
    }
    virtual size_t minNumberOfArgs() const override
    {
        return 1;
    }
    virtual size_t maxNumberOfArgs() const override
    {
        return 1;
    }
};

class ListCommandHandler : public CommandHandler
{
public:
    ListCommandHandler(const std::string& name)
        :
        CommandHandler(name)
    {
    }

    virtual int runCommand(const std::vector<std::string>& args) override
    {
        auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        ActivityLog activityLog(channel);

        std::vector<Activity> activities;
        if (!activityLog.listActivities(std::chrono::system_clock::from_time_t(0), std::chrono::system_clock::now(), activities).ok())
        {
            std::cout << "Failed to list activities\n";
            return -1;
        }

        std::cout << "Retrieved " << std::to_string(activities.size()) << " activities:\n";
        for (const auto& activity : activities)
        {
            std::cout << "\t" <<  activity.id() << " - \"" << activity.name() << "\": " << m_to_km(activity.totalDistance()) << " km, " << activity.totalAscent() << " meters ascent\n";
        }

        return 0;
    }
    virtual std::string syntax() const override
    {
        return name();
    }
    virtual std::string description() const override
    {
        return "lists stored activities";
    }
};

class StatsCommandHandler : public CommandHandler
{
public:
    StatsCommandHandler(const std::string& name)
        :
        CommandHandler(name)
    {
    }

    virtual int runCommand(const std::vector<std::string>& args)
    {
        auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        ActivityLog activityLog(channel);

        activity_log::Stats stats;
        if (!activityLog.calculateStats(std::chrono::system_clock::from_time_t(0), std::chrono::system_clock::now(), stats).ok())
        {
            std::cout << "Failed to calculate stats\n";
            return -1;
        }

        std::cout << "Cummulative activtiy stats:\n";
        std::cout << "\tNumber of activities: " << stats.total_activities() << std::endl;
        std::cout << "\tTime spent: " << stats.total_time() << " seconds" << std::endl;
        std::cout << "\tTotal distance: " << m_to_km(stats.total_distance()) << " km" << std::endl;
        std::cout << "\tTotla climbing: " << stats.total_ascent() << " meters" << std::endl;

        return 0;
    }
    virtual std::string syntax() const
    {
        return name();
    }
    virtual std::string description() const
    {
        return "calculate cummulaive activity statistics";
    }
};

template<typename T>
std::string csvEntry(const T& t)
{
    return std::string(",") + std::to_string(t);
}

class EditCommandHandler : public CommandHandler
{
public:
    EditCommandHandler(const std::string& name)
        :
        CommandHandler(name)
    {
    }

    virtual int runCommand(const std::vector<std::string>& args) override
    {
        auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        ActivityLog activityLog(channel);

        Activity activity;
        auto status = activityLog.getActivity(args[0], activity);
        if (!status.ok())
        {
            std::cout << "Failed to find activity " << args[0] << std::endl;
            return -1;
        }

        status = activity.edit(args[1]);
        if (!status.ok())
        {
            std::cout << "Failed to update activity name " << args[0] << std::endl;
            return -1;
        }

        return 0;
    }
    virtual std::string syntax() const override
    {
        return name() + " <activity_id> <new_activity_name>";
    }
    virtual std::string description() const override
    {
        return "Update the name of an activity";
    }
    virtual size_t minNumberOfArgs() const override
    {
        return 2;
    }
    virtual size_t maxNumberOfArgs() const override
    {
        return 2;
    }
};

class PlotCommandHandler : public CommandHandler
{
public:
    PlotCommandHandler(const std::string& name)
        :
        CommandHandler(name)
    {
    }

    virtual int runCommand(const std::vector<std::string>& args) override
    {
        auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        ActivityLog activityLog(channel);

        Activity activity;
        auto status = activityLog.getActivity(args[0], activity);
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
    virtual std::string syntax() const override
    {
        return name() + " <activity_id>";
    }
    virtual std::string description() const override
    {
        return "plot sctivity data points to CSV format";
    }
    virtual size_t minNumberOfArgs() const override
    {
        return 1;
    }
    virtual size_t maxNumberOfArgs() const override
    {
        return 1;
    }
};

class DeleteCommandHandler : public CommandHandler
{
public:
    DeleteCommandHandler(const std::string& name)
        :
        CommandHandler(name)
    {
    }

    virtual int runCommand(const std::vector<std::string>& args) override
    {
        auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        ActivityLog activityLog(channel);

        Activity activity;
        auto status = activityLog.getActivity(args[0], activity);
        if (!status.ok())
        {
            std::cout << "Failed to find activity " << args[0] << std::endl;
            return -1;
        }

        if (!activity.del().ok())
        {
            std::cout << "Failed to delete activity " << args[0] << std::endl;
            return -1;
        }

        return 0;
    }
    virtual std::string syntax() const override
    {
        return name() + " <activity_id>";
    }
    virtual std::string description() const override
    {
        return "deletes and activity";
    }
    virtual size_t minNumberOfArgs() const override
    {
        return 1;
    }
    virtual size_t maxNumberOfArgs() const override
    {
        return 1;
    }
};

class GetCommandHandler : public CommandHandler
{
public:
    GetCommandHandler(const std::string& name)
        :
        CommandHandler(name)
    {
    }

    virtual int runCommand(const std::vector<std::string>& args) override
    {
        auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        ActivityLog activityLog(channel);

        Activity activity;
        auto status = activityLog.getActivity(args[0], activity);
        if (!status.ok())
        {
            std::cout << "Failed to find activity " << args[0] << std::endl;
            return -1;
        }

        std::cout << "Activity " << activity.id() << std::endl;
        std::cout << "\tName: " << activity.name() << std::endl;
        // TODO: Make human readable
        std::cout << "\tstartTime: " << std::chrono::system_clock::to_time_t(activity.startTime()) << std::endl;
        // TODO: Show as HMS
        std::cout << "\tduration: " << std::chrono::duration_cast<std::chrono::seconds>(activity.duration()).count() << std::endl;
        std::cout << "\ttotalDistance: " << m_to_miles(activity.totalDistance()) << " miles" << std::endl;
        std::cout << "\ttotalAscent: " << m_to_ft(activity.totalAscent()) << " feet" << std::endl;
        std::cout << "\ttotalDescent: " << m_to_ft(activity.totalDescent()) << " feet" << std::endl;
        std::cout << "\taverage_speed: " << mps_to_mph(activity.average_speed()) << " mph" << std::endl;
        std::cout << "\tmax_speed: " << mps_to_mph(activity.max_speed()) << " mph" << std::endl;
        std::cout << "\taverage_heart_rate: " << activity.average_heart_rate() << " bpm" << std::endl;
        std::cout << "\tmax_heart_rate: " << activity.max_heart_rate() << " bpm" << std::endl;
        std::cout << "\taverage_climbing_grade: " << 100.0 * activity.average_climbing_grade() << "%" << std::endl;
        std::cout << "\taverage_descending_grade: " << 100.0 * activity.average_descending_grade() << "%" << std::endl;

        return 0;
    }
    virtual std::string syntax() const override
    {
        return name() + " <activity_id>";
    }
    virtual std::string description() const override
    {
        return "retrieves activity details";
    }
    virtual size_t minNumberOfArgs() const override
    {
        return 1;
    }
    virtual size_t maxNumberOfArgs() const override
    {
        return 1;
    }
};

class CommandRunner
{
public:
    int run(const std::vector<std::string>& args)
    {
        if (args.empty())
        {
            return usage("You must specify a command", -1);
        }

        auto command = args[0];
        std::vector<std::string> params(args.begin() + 1, args.end());

        return runCommand(command, params);
    }

private:
    int runCommand(const std::string& commandName, const std::vector<std::string>& args)
    {
        static const std::vector<std::string> helpCommands{"help", "-h", "-?", "/h", "/?", "--help"};

        if (std::find(helpCommands.begin(), helpCommands.end(), commandName) != helpCommands.end())
        {
            return usage("", 0);
        }

        auto commandEntry = _commands.find(commandName);
        if (commandEntry == _commands.end())
        {
            return usage(std::string("Unknown command \"") + commandName + "\"", -1);
        }

        auto command = commandEntry->second;
        if (args.size() < command->minNumberOfArgs() || args.size() > command->maxNumberOfArgs())
        {
            auto minArgs = command->minNumberOfArgs();
            auto maxArgs = command->maxNumberOfArgs();
            if (minArgs == maxArgs)
            {
                return usage(std::string("Command \"") + commandName + "\" expects " + std::to_string(minArgs) + " arguments", -1);
            }
            return usage(std::string("Command \"") + commandName + "\" expects " + std::to_string(minArgs) + " to " + std::to_string(maxArgs) + " arguments", -1);
        }

        return command->runCommand(args);
    }

    int usage(const std::string& message, int exitCode)
    {
        if (!message.empty())
        {
            std::cout << message << std::endl << std::endl;
        }

        std::cout << "Manages activities on an activity log server\n\n";
        std::cout << "Commands:\n";
        std::cout << "\thelp, -h, -?, --help, \n";
        std::cout << "\t\tshows usage information\n";

        for (auto& [commandName, command]: _commands)
        {
            std::cout << "\t" << command->syntax() << std::endl;
            std::cout << "\t\t" << command->description() << std::endl;
        }

        return exitCode;
    }

    std::map<std::string, std::shared_ptr<CommandHandler> > _commands{
        { "upload", std::make_shared<UploadCommandHandler>("upload") },
        { "list", std::make_shared<ListCommandHandler>("list") },
        { "get", std::make_shared<GetCommandHandler>("get") },
        { "stats", std::make_shared<StatsCommandHandler>("stats") },
        { "plot", std::make_shared<PlotCommandHandler>("plot") },
        { "edit", std::make_shared<EditCommandHandler>("edit") },
        { "delete", std::make_shared<DeleteCommandHandler>("delete") }
    };
};

int main(int argc, char *argv[])
{
    return CommandRunner().run(std::vector<std::string>(argv + 1, argv + argc));
}