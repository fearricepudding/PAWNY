#include "CommandFactory.h"
#include "Command.h"
#include "./commands/raw.h"

using namespace fear::command;

CommandFactory::CommandFactory() {
}

Display* CommandFactory::getDisplay(std::string key) {
    if (key == "raw") {
        return new Raw();
    }
    return new Raw();
}

Command* CommandFactory::getCommand(std::string key) {
    
}

bool CommandFactory::hasDisplay(std::string key) {
    if (key == "show raw") {
        return true;
    }
    return false;
}

bool CommandFactory::hasCommand(std::string key) {
    if (key == "send") {
        return true;
    }
    return false;
}

template <typename T>
void registerCommand(std::string id, T cmd) {
    
}
