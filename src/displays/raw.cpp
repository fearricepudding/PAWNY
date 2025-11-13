#include "raw.h"
#include "ncurses.h"

using namespace fear::command;

Raw::Raw() {
    this->frames = 0;
}

void Raw::consume(canfd_frame frame, Pawny *pawny) {
    pawny->logger->add("New frame");
    this->frames++;
}

void Raw::display(WINDOW* window) {
    mvwprintw(window, 1, 1, "Frames: %i", this->frames);
}
