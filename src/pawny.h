#pragma once

#include "../candy/src/candy.h"

#include <linux/can.h>
#include <queue>
#include <mutex>

#include "server.h"
#include "FrameQueue.h"
#include "File.h"

class Pawny{
public:
    Pawny(bool);
    void init();

     void listen(FrameQueue *);
     void consume(FrameQueue *);

     void broadcast(canfd_frame);
     void enableBroadcast(int port);
     void waitForConnections();
     void enableLogging(std::string path);

     void setBaud(int);
     void setDataRate(int);
     void enableFD();
     void disableFD();
private: 
    Candy *candy;
    bool debug;
    bool store;
    int port;

    int _drate;
    int _baud;
    bool _fd;

    std::string storePath;
    File* logger;
    Server *server;

    bool broadcast_en;
    bool saveToFile;
};
