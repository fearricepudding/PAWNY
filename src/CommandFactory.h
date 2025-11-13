#pragma once

#include "Command.h"
#include "Display.h"

class CommandFactory {
public:
    CommandFactory();
    Command* getCommand(std::string);
    Display* getDisplay(std::string);
    bool hasCommand(std::string);
    bool hasDisplay(std::string);

    template <typename T>
    void registerCommand(std::string, T);

private:
    std::map<std::string, Command*> commands;
};
