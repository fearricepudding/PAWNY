#pragma once

#include <linux/can.h>
#include <queue>
#include <mutex>


class FrameQueue {
public:
    std::queue<canfd_frame> _frames;
    std::mutex _m;
};
