#include "pawny.h"
#include "candy.h"
#include "server.h"
#include "File.h"
#include "Logger.h"

#include <linux/can.h>
#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <boost/thread.hpp>
#include <stdlib.h>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 
#include <ncurses.h>
#include <stdlib.h>

#include "./displays/raw.h"

Pawny::Pawny(bool debug, Logger *logger) {
    this->logger = logger;
    this->debug = debug;
    this->store = false;
    this->broadcast_en = false;

    this->_drate = 2000000;
    this->_baud = 500000;
    this->_fd = false;

};

void Pawny::setDataRate(int drate) {
    this->_drate = drate;
}

void Pawny::setBaud(int brate) {
    this->_baud = brate;
}

void Pawny::enableFD() {
    this->_fd = true;
}

void Pawny::disableFD() {
    this->_fd = false;
}

void Pawny::init(){
    this->logger->add("[*] Starting ECUPWN PAWNY");

    if (this->_fd) {
        this->candy = new Candy(this->debug, this->_baud, this->_drate, this->logger);
    } else {
        this->candy = new Candy(this->debug, this->_drate, this->logger);
    };

    this->candy->setup();
    
    if(!this->candy->isConnected()){
        this->logger->add("[*] CAN error, exiting");
        exit(1);
    };

    if (this->broadcast_en) {
        this->server = new Server(this->port);
        this->logger->add("[*] Broadcast enabled: "+this->port);
    }

    if (this->store) {
        this->fileLog = new File(storePath);
        this->logger->add("[*] Log file enabled: "+this->storePath);
    };

    this->logger->add("[*] PAWNY ready");
}

void Pawny::enableLogging(std::string path) {
    this->store = true;
    this->storePath = path;
}

void Pawny::enableBroadcast(int port) {
    this->port = port;
    this->broadcast_en = true;
}

void Pawny::listen(FrameQueue *queue){
    if(!this->candy->isConnected()){
        return;
    };

    int debugFrames = 0;

    while(1){
        canfd_frame frame = this->candy->recieve();
        queue->add(frame);
        
        if (this->debug) {
            debugFrames++;
            if (debugFrames >= 12) {
                return;
            };
            boost::this_thread::sleep_for(boost::chrono::milliseconds(2000));
        };
    };
};

void Pawny::consume(canfd_frame frame){
    if (this->broadcast_en && this->server->isConnected()) {
        this->broadcast(frame);
    }

    if (this->store) {
        this->fileLog->write(frame);
    }
}

void Pawny::rawLogFrame(canfd_frame frame) {
    std::stringstream frameString;
    frameString << "0x" << std::hex << frame.can_id << "[";
    for (int i = 0; i < 8; i++) {
        frameString << "{" << (void*)(frame.data[i]) << "}";
    }
    frameString << "]";
    //std::cout << frameString.str() << std::endl;
    this->logger->add("New frame: "+frameString.str());
}

void Pawny::rawConsume(FrameQueue *queue) {
    while(1){
        if (!queue->isEmpty()) {
            canfd_frame frame = queue->pop();
            Pawny::rawLogFrame(frame);
            Pawny::consume(frame);
        }
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    }
}

void Pawny::broadcast(canfd_frame frame) {
    this->logger->add("[#] Got new broadcast connection");
    server->sendFrame(frame);
};

void Pawny::waitForConnections() {
    server->waitForConnection();
}

