#pragma once

#include <stdlib.h>
#include <iostream>
#include <queue>
#include <ncurses.h>
#include <list>
#include "FrameQueue.h"
#include "pawny.h"

struct State {
    int bufferSize;
    std::queue<std::string> commandHistory;
    std::string command;
};

class Interface {
public:
    Interface(Pawny*);
    void setupInteractive();

    void display(FrameQueue*);
    void input();

private: 
    Pawny* pawny;
    State state;
    WINDOW* w_logo;
    WINDOW* w_commandHistory;
    WINDOW* w_stats;
    WINDOW* w_output;

    bool updating;

    void runCommand();
    void update();
    void setupWindow(WINDOW*, int, int, int, int);
    
    void renderLogo();
    void renderOutput();
    void renderHistory();
    void renderStats();
};
