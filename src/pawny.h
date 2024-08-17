#pragma once

#include "../candy/src/candy.h"

#include <linux/can.h>
#include <queue>
#include <mutex>

class Pawny{
public:
    Pawny();
    void init();
private: 
    Candy *candy;
    std::queue<can_frame> _frames;
    std::mutex _m;

    void listen();
    void broadcast();
};
