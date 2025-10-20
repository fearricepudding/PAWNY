#pragma once

#include "../candy/src/candy.h"

#include <linux/can.h>
#include <queue>
#include <mutex>
#include <map>
#include <list>

#include "server.h"
#include "FrameQueue.h"
#include "File.h"

class Pawny{
public:
    bool broadcast_en;
    bool saveToFile;

    Pawny(bool);
    void init();

     void listen(FrameQueue *);
     void consume(FrameQueue *);
     void display(FrameQueue *);
     void input();

     void broadcast(canfd_frame);
     void enableBroadcast(int port);
     void waitForConnections();
     void enableLogging(std::string path);
     void setupInteractive();

     void setBaud(int);
     void setDataRate(int);
     void enableFD();
     void disableFD();
private: 
    std::string storePath;
    File* logger;
    Server *server;

    Candy *candy;
    bool debug;
    bool store;
    int port;

    int _drate;
    int _baud;
    bool _fd;
};
