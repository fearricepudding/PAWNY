#pragma once

#include "../candy/src/candy.h"

#include <linux/can.h>
#include <queue>
#include <mutex>

#include "FrameQueue.h"

class Pawny{
public:
    Pawny(bool, int, int);
    void init();

     void listen(FrameQueue *);
     void broadcast(FrameQueue *, int);

private: 
    Candy *candy;
    bool debug;
};
