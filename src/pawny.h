#pragma once

#include "../candy/src/candy.h"

#include <linux/can.h>
#include <queue>
#include <mutex>

class Pawny{
public:
    Pawny();
    Pawny(bool);
    void init();

    void listen();
    void broadcast();

private: 
    Candy *candy;
    std::queue<can_frame> _frames;
    std::mutex _m;
    bool debug;
};
