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

    Pawny(bool);

    int port;
    bool broadcast_en;
    bool saveToFile;
    bool debug;
    bool store;
    int _drate;
    int _baud;
    bool _fd;
    std::string storePath;

    void init();

     void listen(FrameQueue *);
     void consume(canfd_frame);
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
    File* logger;
    Server *server;
    Candy *candy;
};
