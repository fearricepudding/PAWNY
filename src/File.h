#pragma once

#include <linux/can.h>
#include <iostream>
#include <fstream>

class File {
public: 
    File(std::string);
    File(std::string, std::string);
    void create();
    bool exists();
    void write(canfd_frame);
private:
    std::string _path;
    std::ofstream ostrm;
    std::string frameString(canfd_frame);
    std::ofstream* streamF;
};
