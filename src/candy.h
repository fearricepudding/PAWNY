#pragma once

#include <string>
#include <linux/can.h>

class Candy {
public: 
    Candy();
    Candy(int);
    Candy(bool);
    Candy(bool, int);
    Candy(int, int);
    Candy(bool, int, int);
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
};
