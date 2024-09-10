#pragma once

#include <linux/can.h>
#include <queue>
#include <mutex>


class FrameQueue {
public:
    std::queue<can_frame> _frames;
    std::mutex _m;
};