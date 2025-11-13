#pragma once

#include <list>
#include <map>
#include <string>

namespace fear{
struct Log {
    std::string message;
    int colorPair;
};

class Logger {
public:
    Logger();

    std::list<Log> getLogs();
    void add(Log);
    void add(std::string);
    void add(std::string, int);
private:
    std::list<Log> logs;
    int logSize;
};
};
