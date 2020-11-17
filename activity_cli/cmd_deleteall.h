#pragma once

#include "cmd.h"


class DeleteAllCommandHandler : public CommandHandler
{
public:
    DeleteAllCommandHandler(const std::string& name);

    int runCommand(const std::vector<std::string>& args) override;
    std::string syntax() const override;
    std::string description() const override;
};
