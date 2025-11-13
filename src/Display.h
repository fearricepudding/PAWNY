#pragma once

#include <linux/can.h>
#include <ncurses.h>
#include "pawny.h"

class Display {
public:
    virtual void consume(canfd_frame, Pawny*) = 0;
    virtual void display(WINDOW*) = 0;
};
