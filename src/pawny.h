#pragma once

#include "../candy/src/candy.h"

#include <linux/can.h>
#include <queue>
#include <mutex>

#include "FrameQueue.h"

class Pawny{
public:
    Pawny();
    Pawny(bool);
    void init();

     void listen(FrameQueue *);
     void broadcast(FrameQueue *);

private: 
    Candy *candy;
    bool debug;
};
