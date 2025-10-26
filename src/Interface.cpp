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

    if (has_colors() == FALSE) {	
        endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}

    start_color();

    // Setup colours
    init_pair (0, COLOR_WHITE, COLOR_BLACK);
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

void Interface::consume(FrameQueue* queue) {
    while(1){
        if (queue->isEmpty()) {
            continue;
        }
        canfd_frame frame = queue->pop();
        this->pawny->consume(frame);
    };
}

void Interface::display(FrameQueue* queue) {
    w_logo = newwin(6, 35, 0, 1);
    w_stats = newwin(7, (COLS/3*2)-35, 0, 35);
    w_log = newwin(0, (COLS/3), 0, (COLS/3*2));
    w_output = newwin((LINES - (10)), (COLS/3)*2, 7, 0);
    w_commandHistory = newwin(3, (COLS/3)*2, LINES-3, 0);

    refresh();
    this->renderLogo();
    this->renderLog();

    while (1) {
        int bufferSize = queue->size();
        this->state.bufferSize = bufferSize;
        this->update();
        boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
}

void Interface::renderLogo() {
    mvwprintw(w_logo, 0, 0, "______  ___  _    _ _   ___   __");
    mvwprintw(w_logo, 1, 0, "| ___ \\/ _ \\| |  | | \\ | \\ \\ / /");
    mvwprintw(w_logo, 2, 0, "| |_/ / /_\\ \\ |  | |  \\| |\\ V / ");
    mvwprintw(w_logo, 3, 0, "|  __/|  _  | |/\\| | . ` | \\ /  ");
    mvwprintw(w_logo, 4, 0, "| |   | | | \\  /\\  / |\\  | | |  ");
    mvwprintw(w_logo, 5, 0, "\\_|   \\_| |_/\\/  \\/\\_| \\_/ \\_/  ");
    wrefresh(w_logo);
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

void Interface::renderLog() {
    wclear(w_log);
    box(w_log, 0, 0);
    mvwprintw(w_log, 0, 2, " Log ");

    int limit = LINES-2;
    int count = 0;
    for (std::string value : this->state.log) {
        mvwprintw(w_log, limit-count, 1, "%s", value.c_str());
        count++;
    }

    wrefresh(w_log);
}

void Interface::renderOutput() {
    wclear(w_output);
    box(w_output, 0, 0);

    mvwprintw(w_output, 0, 2, " Output ");

    wrefresh(w_output);
}

void Interface::renderHistory() {
    wclear(w_commandHistory);
    box(w_commandHistory, 0, 0);

    wattron(w_commandHistory, COLOR_PAIR(1));
    mvwprintw(w_commandHistory, 1, 2, ">");
    wattroff(w_commandHistory, COLOR_PAIR(1));
    mvwprintw(w_commandHistory, 1, 4, "%s", this->state.command.c_str());

    /*
    std::string lastCommand = "";
    if (this->state.commandHistory.size() > 0) {
        lastCommand = this->state.commandHistory.back();
    }
    mvwprintw(w_commandHistory, 3, 1, "> %s", lastCommand.c_str());
    */

    wrefresh(w_commandHistory);
}

void Interface::renderStats() {
    wclear(w_stats);
    box(w_stats, 0, 0);

    std::string version = "Pawny v0.2.1";
    if (this->pawny->debug) {
        version += " [Debug mode]";
    }
    mvwprintw(w_stats, 1, 1, "%s", version.c_str());

    mvwprintw(w_stats, 2, 1, "Buffer size: %d frames (%ld bytes)", this->state.bufferSize, this->state.bufferSize*(sizeof(canfd_frame)));

    std::string broadcast = "Disabled";
    if (this->pawny->broadcast_en) {
        broadcast = "Enabled (0.0.0.0:8047)";
    }
    mvwprintw(w_stats, 3, 1, "Broadcast: %s", broadcast.c_str());

    std::string storeStat = "Disabled";
    if (this->pawny->store) {
        storeStat = "Enabled [" + this->pawny->storePath + "]";
    }
    mvwprintw(w_stats, 4, 1, "File logging: %s", storeStat.c_str());

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
    this->state.log.push_front("> '"+cmd+"'");
    this->state.log.push_front("Command not found");
    this->state.command = "";
    this->renderLog();
}
