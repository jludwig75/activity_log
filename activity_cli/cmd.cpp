#include "cmd.h"

CommandHandler::CommandHandler(const std::string& name)
    :
    _name(name)
{
}

const std::string& CommandHandler::name() const
{
    return _name;
}
