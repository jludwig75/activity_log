#pragma once

#include "cmd.h"


class StatsCommandHandler : public CommandHandler
{
public:
    StatsCommandHandler(const std::string& name);

    int runCommand(const std::vector<std::string>& args) override;
    std::string syntax() const override;
    std::string description() const override;
};

