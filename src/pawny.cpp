#include "pawny.h"
#include "../candy/src/candy.h"
#include "server.h"
#include "File.h"

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

Pawny::Pawny(bool debug) {
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
    //std::cout << "[*] Starting ECUPWN PAWNY" << std::endl;

    if (this->_fd) {
        this->candy = new Candy(this->debug, this->_baud, this->_drate);
    } else {
        this->candy = new Candy(this->debug, this->_drate);
    };

    this->candy->setup();
    
    if(!this->candy->isConnected()){
        //std::cout << "[*] CAN error, exiting" << std::endl;
        exit(1);
    };

    if (this->broadcast_en) {
        this->server = new Server(this->port);
    }

    if (this->store) {
        this->storePath = storePath;
        this->logger = new File(storePath);
    };
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
            boost::this_thread::sleep_for(boost::chrono::milliseconds(20));
        };
    };
};

void Pawny::consume(FrameQueue *queue){
     while(1){

        if (queue->isEmpty()) {
            continue;
        }

        canfd_frame frame = queue->pop();

        if (this->broadcast_en && this->server->isConnected()) {
            this->broadcast(frame);
        }

        if (this->store) {
            this->logger->write(frame);
        }

    };
}

void Pawny::broadcast(canfd_frame frame) {
    //std::cout << "[#] Got connection" << std::endl;
    server->sendFrame(frame);
};

void Pawny::waitForConnections() {
    server->waitForConnection();
}

