#include "./Interface.h"
#include "pawny.h"
#include "FrameQueue.h"
#include "Logger.h"

#include <stdlib.h>
#include <iostream>
#include <queue>
#include <list>
#include <ncurses.h>
#include <boost/thread.hpp>

#include "./displays/raw.h"
#include "Command.h"
#include "Display.h"
#include "CommandFactory.h"
#include <sstream>

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
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    
    // setup default state values
    this->state.command = "";
    this->state.bufferSize = 0;


    this->commandFactory = new CommandFactory();
    this->currentDisplay = commandFactory->getDisplay("placeholder");
}

void Interface::consume(FrameQueue* queue) {
    while(1){
        if (queue->isEmpty()) {
            continue;
        }
        canfd_frame frame = queue->pop();
        this->currentDisplay->consume(frame, this->pawny);
        this->pawny->consume(frame);
        this->update();
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
    this->renderHistory();

    this->update();
    /*
    while (1) {
        int bufferSize = queue->size();
        this->state.bufferSize = bufferSize;
        this->update();
        boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    }
    */
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
    this->pawny->updating.lock();
    this->renderStats();
    this->renderOutput();
    this->renderLog();
    this->pawny->updating.unlock();
}

void Interface::renderLog() {
    wclear(w_log);
    box(w_log, 0, 0);
    mvwprintw(w_log, 0, 2, " Log ");

    std::list<Log> logs = this->pawny->logger->getLogs();
    int limit = LINES-2;
    int count = 0;
    for (Log log : logs) {
        wattron(w_log, COLOR_PAIR(log.colorPair));
        mvwprintw(w_log, limit-count, 1, "%s", log.message.c_str());
        wattroff(w_log, COLOR_PAIR(log.colorPair));
        count++;
    }

    wrefresh(w_log);
}

void Interface::renderOutput() {
    wclear(w_output);
    box(w_output, 0, 0);

    mvwprintw(w_output, 0, 2, " Output ");

    this->currentDisplay->display(w_output);

    wrefresh(w_output);
}

void Interface::renderHistory() {
    wclear(w_commandHistory);
    box(w_commandHistory, 0, 0);

    wattron(w_commandHistory, COLOR_PAIR(1));
    mvwprintw(w_commandHistory, 1, 2, ">");
    wattroff(w_commandHistory, COLOR_PAIR(1));
    mvwprintw(w_commandHistory, 1, 4, "%s", this->state.command.c_str());

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
        this->pawny->updating.lock();
        this->renderHistory();
        this->pawny->updating.unlock();
    }
}
 
void Interface::runCommand() {
    std::string cmd = this->state.command;
    this->pawny->logger->add("[>] "+cmd, 1);
    if (this->startsWith(&cmd, "show")) {
        if (this->commandFactory->hasDisplay(cmd)) {
            this->currentDisplay = this->commandFactory->getDisplay(cmd);
        } else {
            this->pawny->logger->add("[!] Display not found", 2);
        }
    } else {
        if (this->commandFactory->hasCommand(cmd)) {
            Command* toRun = this->commandFactory->getCommand(cmd);
            toRun->run();
        } else {
            this->pawny->logger->add("[!] Command not found", 2);
        }
    }
    this->state.command = "";
    this->pawny->updating.lock();
    this->renderHistory();
    this->renderLog();
    this->renderOutput();
    this->pawny->updating.unlock();
}

bool Interface::startsWith(std::string* haystack , std::string needle) {
    int needleSize = needle.size();
    if (haystack->size() < needleSize) {
        return false;
    }
    std::string haystackString = haystack->substr(0, needleSize);
    if (haystackString == needle) {
        return true;
    }
    return false;
}
