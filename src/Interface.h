#pragma once

#include <stdlib.h>
#include <iostream>
#include <queue>
#include <ncurses.h>
#include <list>
#include <boost/thread.hpp>

#include "FrameQueue.h"
#include "pawny.h"
#include "Logger.h"
#include "Command.h"
#include "Display.h"
#include "CommandFactory.h"

using namespace fear;

struct State {
    int bufferSize;
    std::string command;
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

    boost::mutex updating;

    void runCommand();
    void update();
    
    void renderLogo();
    void renderOutput();
    void renderHistory();
    void renderStats();
    void renderLog();

    Display* currentDisplay;
    CommandFactory* commandFactory;
    
    bool startsWith(std::string*, std::string);
};
