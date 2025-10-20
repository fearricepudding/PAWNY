#include "./Interface.h"
#include "pawny.h"
#include "FrameQueue.h"
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <list>
#include <ncurses.h>
#include <boost/thread.hpp>

Interface::Interface(Pawny* pawny) {
    this->pawny = pawny;
};

void Interface::setupInteractive() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();

    // Setup colours
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    
    // setup default state values
    this->state.command = "";
    this->state.bufferSize = 0;

}

void Interface::setupWindow(WINDOW* local_win, int height, int width, int starty, int startx) {
    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);
    wrefresh(local_win);
}

void Interface::display(FrameQueue* queue) {
    w_stats = newwin(5, COLS/2, 0, 0);
    w_commandHistory = newwin(5, COLS, 5, 0);
    w_output = newwin((LINES - (10)), COLS, 10, 0);
    refresh();

    while (1) {
        int bufferSize = queue->size();
        this->state.bufferSize = bufferSize;
        this->update();
        boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
}

void Interface::update() {
    if (this->updating) {
        return;
    }
    this->updating = true;
    this->renderStats();
    this->renderHistory();
    this->renderOutput();
    this->updating = false;
}

void Interface::renderOutput() {
    wclear(w_output);
    box(w_output, 0, 0);

    wrefresh(w_output);
}

void Interface::renderHistory() {
    wclear(w_commandHistory);
    box(w_commandHistory, 0, 0);

    mvwprintw(w_commandHistory, 0, 2, "Command");

    attron(COLOR_PAIR(1));
    mvwprintw(w_commandHistory, 1, 1, "Command: %s", this->state.command.c_str());

    wrefresh(w_commandHistory);
}

void Interface::renderStats() {
    wclear(w_stats);
    box(w_stats, 0, 0);

    mvwprintw(w_stats, 1, 1, "Wake up, Neo...");
    mvwprintw(w_stats, 2, 1, "Buffer size: %d frames (%ld bytes)", this->state.bufferSize, this->state.bufferSize*(sizeof(canfd_frame)));

    std::string broadcast = "Disabled";
    if (this->pawny->broadcast_en) {
        broadcast = "Enabled";
    }
    mvwprintw(w_stats, 3, 1, "Broadcast address: %s", broadcast.c_str());

    wrefresh(w_stats);
}

void Interface::input() {
    int ch;
    while (ch = getch()) {
        switch(ch) {
            case 10:
                this->runCommand();
                break;
            case 263:
                if (this->state.command != "") {
                    this->state.command = this->state.command.substr(0, this->state.command.length()-1);
                };
                break;
            default:
                if (this->state.command.length() <= 50) {
                    const char ascii = static_cast<char>(ch);
                    this->state.command += ascii;
                }
        }
        this->update();
    }
}
 

void Interface::runCommand() {
    std::string cmd = this->state.command;
    this->state.command = "";
    this->state.commandHistory.push_back(cmd);
}
