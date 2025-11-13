#pragma once

#include <linux/can.h>

class Command {
public: 
    virtual void run() = 0;
};