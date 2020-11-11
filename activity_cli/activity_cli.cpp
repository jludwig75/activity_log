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
    ~CommandHandler() {}
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
            std::cout << "Failed to upload activity \"" + args[0] + "\"\n";
            return -1;
        }

        std::cout << "Successfully uploaded activity \"" + args[0] + "\"\n";

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

    virtual int runCommand(const std::vector<std::string>& args)
    {
        auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        ActivityLog activityLog(channel);

        std::vector<Activity> activities;
        activityLog.listActivities(std::chrono::system_clock::from_time_t(0), std::chrono::system_clock::now(), activities);
        {
            std::cout << "Failed to list activities\n";
            return -1;
        }

        std::cout << "Retrieved " << std::to_string(activities.size()) << " activities:\n";
        for (const auto& activity : activities)
        {
            std::cout << "\t" << activity.name() << ": " << m_to_km(activity.totalDistance()) << " km " << activity.totalAscent() << " meters ascent\n";
        }

        return 0;
    }
    virtual std::string syntax() const
    {
        return name();
    }
    virtual std::string description() const
    {
        return "lists stored activities";
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
        { "list", std::make_shared<ListCommandHandler>("list") }
    };
};

int main(int argc, char *argv[])
{
    return CommandRunner().run(std::vector<std::string>(argv + 1, argv + argc));
}