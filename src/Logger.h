#pragma once

#include <list>
#include <map>
#include <string>
#include <iostream>

namespace fear{
struct Log {
    std::string message;
    int colorPair;
};

class Logger {
public:
    Logger();
    Logger(bool);
    std::list<Log> getLogs();
    void add(Log);
    void add(std::string);
    void add(std::string, int);
private:
    std::list<Log> logs;
    int logSize;
    bool raw;
};
};
