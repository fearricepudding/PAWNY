#pragma once

#include <stdlib.h>
#include <iostream>
#include <queue>
#include <ncurses.h>
#include "pawny.h"

struct State {
    int bufferSize;
    std::list<std::string> commandHistory;
};

class Interface {
public:
    Interface();
    void setupInteractive();

    void display(FrameQueue*, Pawny*);
    void input(Pawny*);

private: 
    std::string command;
    void update();
    State state;
};
