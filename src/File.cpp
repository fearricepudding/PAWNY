#include "File.h"
#include <fstream>
#include <boost/filesystem.hpp>
#include <linux/can.h>
#include <sstream>

File::File(std::string fileName, std::string path) {
    this->_path = path+"/"+fileName;
    this->ostrm = std::ofstream(this->_path, std::ios::binary);
};

File::File(std::string path) {
    this->_path = path;
//    this->ostrm.open(this->_path, std::ofstream::out | std::ofstream::app);
}

void File::create() {
    //
};

bool File::exists() {
    if (boost::filesystem::exists(this->_path)) {
        return true;
    };
    return false;
};

void File::write(canfd_frame frame) {
    std::string frameDataString = this->frameString(frame);
    const char* frameData = frameDataString.c_str();
    std::ofstream stream;
    stream.open(this->_path, std::ios_base::app);
    stream << frameDataString << "\n"; 
};

std::string File::frameString(canfd_frame frame) {
    std::stringstream frameString;
    frameString << "0x" << std::hex << frame.can_id << "[";
    for (int i = 0; i < 8; i++) {
        frameString << "{" << (void*)(frame.data[i]) << "}";
    }
    frameString << "]";
    return frameString.str();
};


