#include "raw.h"

#include <ncurses.h>
#include <sstream>
#include <string>
#include <list>

using namespace fear::command;

Raw::Raw() {
    this->frames = 0;
}

void Raw::consume(canfd_frame frame, Pawny *pawny) {
    this->frames++;
    std::stringstream frameString;
    frameString << "0x" << std::hex << frame.can_id << " [";
    for (int i = 0; i < 8; i++) {
        frameString << " " << (void*)(frame.data[i]);
    }
    frameString << " ]";
    std::string result = frameString.str();
    this->windowbuffer.push_front(result);
}

void Raw::display(WINDOW* window) {
    int bottom = LINES-12;
    int i = 0;
    for (std::string frame: this->windowbuffer) {
        mvwprintw(window, bottom-i, 3, "%s", frame.c_str());
        i++;
    }
}
