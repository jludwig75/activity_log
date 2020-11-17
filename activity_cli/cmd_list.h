#pragma once

#include "cmd.h"


class ListCommandHandler : public CommandHandler
{
public:
    ListCommandHandler(const std::string& name);

    int runCommand(const std::vector<std::string>& args) override;
    std::string syntax() const override;
    std::string description() const override;
};

