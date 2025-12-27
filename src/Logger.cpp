#include "Logger.h"

#include <string> 
#include <map>
#include <list>

using namespace fear;

Logger::Logger() {
    this->logSize = 100;
}

Logger::Logger(bool raw) {
    this->raw = raw;
}

void Logger::add(Log log) {
    if (this->raw) {
        std::string message = log.message;
        std::cout << message << std::endl;
        return;
    }
    logs.push_front(log);
}

void Logger::add(std::string message) {
    Log* log = new Log();
    log->message = message;
    log->colorPair = 0;
    this->add(*log);
}

void Logger::add(std::string message, int pair) {
    Log* log = new Log();
    log->message = message;
    log->colorPair = pair;
    this->add(*log);
}

std::list<Log> Logger::getLogs() {
    return this->logs;
}
