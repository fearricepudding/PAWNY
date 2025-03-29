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

Pawny::Pawny(bool debug, int bitrate, int port, bool fd, int datarate, bool store, std::string storePath) {
    this->debug = debug;
    this->store = store;
    if (store) {
        this->storePath = storePath;
        this->logger = new File(storePath);
    };

    if (fd) {
        this->candy = new Candy(debug, bitrate, datarate);
    } else {
        this->candy = new Candy(debug, bitrate);
    };
};

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
        canfd_frame frame = this->candy->recieve();
        queue->_m.lock();
        std::cout << "[%] Pushing frame" << std::endl;
        queue->_frames.push(frame);
        std::cout << "[%] Frame queue: " << queue->_frames.size() << std::endl;
        queue->_m.unlock();
        
        if (this->store) {
            this->logger->write(frame);
        };

        if (this->debug) {
            debugFrames++;
            if (debugFrames >= 100) {
                return;
            };
            boost::this_thread::sleep_for(boost::chrono::milliseconds(20));
        };
    };
};

void Pawny::broadcast(FrameQueue *queue, int port){
    Server *server = new Server(port);
    server->waitForConnection();
    std::cout << "[#] Got connection, sending frames loop" <<std::endl;
    while(1){
        canfd_frame frame;
        queue->_m.lock();
        if(queue->_frames.size() <= 0){
            queue->_m.unlock();
            continue;
        };
        frame = (canfd_frame) queue->_frames.front();
        queue->_frames.pop();
        queue->_m.unlock();

        std::cout << "[#] Got frame, sending" << std::endl;
        server->sendFrame(frame);
        std::cout << "[#] Sent" << std::endl;
    };
};
