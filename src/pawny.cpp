#include "pawny.h"
#include "../candy/src/candy.h"
#include "server.h"

#include <linux/can.h>
#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <boost/thread.hpp>
#include <stdlib.h>

Pawny::Pawny(){
    this->candy = new Candy();
}

void Pawny::init(){
    std::cout << "[*] Starting ECUPWN PAWNY" << std::endl;
    this->candy->setup();
    
    if(!this->candy->isConnected()){
        std::cout << "[*] CAN error, exiting" << std::endl;
        exit(1);
    };
};

void Pawny::listen(){
    if(!this->candy->isConnected()){
        return;
    };

    while(1){
        can_frame frame = this->candy->recieve();
        this->_m.lock();
	std::cout << "[%] Pushing frame" << std::endl;
        this->_frames.push(frame);
	std::cout << "[%] Frame queue: " << this->_frames.size() << std::endl;
        this->_m.unlock();
    };
};

void Pawny::broadcast(){
    Server *server = new Server();
    server->waitForConnection();
    std::cout << "[#] Connected!" <<std::endl;
    while(1){
        can_frame frame;
        this->_m.lock();
        if(this->_frames.size() <= 0){
	    this->_m.unlock();
            continue;
        };
        frame = (can_frame) this->_frames.front();
        this->_frames.pop();
        this->_m.unlock();

	std::cout << "[#] Got frame, sending" << std::endl;
        server->sendFrame(frame);
	std::cout << "[#] Sent" << std::endl;
    };
};
