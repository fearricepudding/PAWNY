#pragma once

#include "../Display.h"

#include <linux/can.h>
#include <ncurses.h>
#include <list>
#include <string>

namespace fear::command{
class Raw: public Display {
public:
    Raw();
    int frames;

    std::list<std::string> windowbuffer;
    void consume(canfd_frame, Pawny*);
    void display(WINDOW*);
};
};
