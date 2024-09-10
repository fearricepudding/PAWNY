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
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

Pawny::Pawny(){
    this->debug = false;
    this->candy = new Candy();
}

Pawny::Pawny(bool debug) {
    this->debug = debug;
    this->candy = new Candy(debug);
}

void Pawny::init(){
    std::cout << "[*] Starting ECUPWN PAWNY" << std::endl;
    this->candy->setup();
    
    if(!this->candy->isConnected()){
        std::cout << "[*] CAN error, exiting" << std::endl;
        exit(1);
    };
};

void Pawny::listen(FrameQueue *queue){
    if(!this->candy->isConnected()){
        return;
    };

    int debugFrames = 0;

    while(1){
        can_frame frame = this->candy->recieve();
        queue->_m.lock();
        std::cout << "[%] Pushing frame" << std::endl;
        queue->_frames.push(frame);
        std::cout << "[%] Frame queue: " << queue->_frames.size() << std::endl;
        queue->_m.unlock();
        if (this->debug) {
            debugFrames++;
            if (debugFrames >= 10) {
                return;
            };
            boost::this_thread::sleep_for(boost::chrono::milliseconds(2000));
        };
    };
};

void Pawny::broadcast(FrameQueue *queue){
    Server *server = new Server();
    server->waitForConnection();
    std::cout << "[#] Got connection, sending frames loop" <<std::endl;
    while(1){
        can_frame frame;
        queue->_m.lock();
        if(queue->_frames.size() <= 0){
            queue->_m.unlock();
            continue;
        };
        frame = (can_frame) queue->_frames.front();
        queue->_frames.pop();
        queue->_m.unlock();

        std::cout << "[#] Got frame, sending" << std::endl;
        server->sendFrame(frame);
        std::cout << "[#] Sent" << std::endl;
    };
};
