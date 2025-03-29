#pragma once

#include "../candy/src/candy.h"

#include <linux/can.h>
#include <queue>
#include <mutex>

#include "FrameQueue.h"
#include "File.h"

class Pawny{
public:
    Pawny(bool, int, int, bool, int, bool, std::string);
    void init();

     void listen(FrameQueue *);
     void broadcast(FrameQueue *, int);

private: 
    Candy *candy;
    bool debug;
    bool store;
    std::string storePath;
    File* logger;
};
