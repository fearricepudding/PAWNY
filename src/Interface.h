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
    std::string command;
    std::list<std::string> log;
};

class Interface {
public:
    Interface(Pawny*);
    void setupInteractive();

    void consume(FrameQueue*);
    void display(FrameQueue*);
    void input();

private: 
    Pawny* pawny;
    State state;
    WINDOW* w_logo;
    WINDOW* w_commandHistory;
    WINDOW* w_stats;
    WINDOW* w_output;
    WINDOW* w_log;

    bool updating;

    void runCommand();
    void update();
    void setupWindow(WINDOW*, int, int, int, int);
    
    void renderLogo();
    void renderOutput();
    void renderHistory();
    void renderStats();
    void renderLog();
};
