#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "cmd_delete.h"
#include "cmd_deleteall.h"
#include "cmd_download.h"
#include "cmd_edit.h"
#include "cmd_get.h"
#include "cmd_list.h"
#include "cmd_plot.h"
#include "cmd_stats.h"
#include "cmd_track.h"
#include "cmd_upload.h"


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
        if (args.size() < command->minNumberOfArgs() || (command->maxNumberOfArgs() != -1 && args.size() > command->maxNumberOfArgs()))
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
        { "delete", std::make_shared<DeleteCommandHandler>("delete") },
        { "deleteall", std::make_shared<DeleteAllCommandHandler>("deleteall") },
        { "download", std::make_shared<DownloadCommandHandler>("download") },
        { "edit", std::make_shared<EditCommandHandler>("edit") },
        { "get", std::make_shared<GetCommandHandler>("get") },
        { "list", std::make_shared<ListCommandHandler>("list") },
        { "plot", std::make_shared<PlotCommandHandler>("plot") },
        { "stats", std::make_shared<StatsCommandHandler>("stats") },
        { "track", std::make_shared<TrackCommandHandler>("track") },
        { "upload", std::make_shared<UploadCommandHandler>("upload") }
    };
};

int main(int argc, char *argv[])
{
    return CommandRunner().run(std::vector<std::string>(argv + 1, argv + argc));
}