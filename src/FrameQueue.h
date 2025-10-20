#pragma once

#include <linux/can.h>
#include <queue>
#include <mutex>


class FrameQueue {
public: 
    void add(canfd_frame);
    int size();
    bool isEmpty();
    canfd_frame pop();
private:
    std::queue<canfd_frame> _frames;
    std::mutex _m;
};
