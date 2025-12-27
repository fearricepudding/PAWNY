#pragma once

#include <string>
#include <linux/can.h>
#include "Logger.h"

using namespace fear;

class Candy {
public: 
    Candy(bool, int, Logger*);
    Candy(bool, int, int, Logger*);
    ~Candy();
    canfd_frame recieve();
    int send();
    void setup();
    void shutdown();
    std::string getError();
    bool isConnected();

private:
    int setupCanLink();
    void closeCanLink();
    std::string error;
    bool _connected;
    int s;
    bool _debug;
    bool _fd;
    int _bitrate;
    int _datarate;
    Logger* logger;
};
