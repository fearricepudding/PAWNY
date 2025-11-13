#pragma once

#include "../Display.h"

#include <linux/can.h>
#include <ncurses.h>

namespace fear::command{
class Raw: public Display {
public:
    Raw();
    int frames;
    void consume(canfd_frame, Pawny*);
    void display(WINDOW*);
private:
    std::map<std::string, canfd_frame> windowbuffer;
};
};
