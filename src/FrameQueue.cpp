#include <linux/can.h>
#include <queue>
#include <mutex>
#include <iostream>

#include "FrameQueue.h"

void FrameQueue::add(canfd_frame frame) {
    this->_frames.push(frame);
}

int FrameQueue::size() {
    return this->_frames.size();
}

bool FrameQueue::isEmpty() {
    return this->_frames.empty();
}

canfd_frame FrameQueue::pop() {
    canfd_frame frame = (canfd_frame) this->_frames.front();
    this->_frames.pop();
    return frame;
}
