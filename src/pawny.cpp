#include "pawny.h"
#include "../candy/src/candy.h"

#include <linux/can.h>
#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <boost/thread.hpp>

Pawny::Pawny(){
    this->candy = new Candy();
}

void Pawny::init(){
    std::cout << "[*] Starting ECUPWN PAWNY" << std::endl;
    this->candy->setup();
    if(this->candy->isConnected()){
	boost::thread t_canListener(&Pawny::listen, this);
        boost::thread t_canBroadcaster(&Pawny::broadcast, this);
    	t_canListener.join();
        t_canBroadcaster.join();
    };
};

void Pawny::listen(){
    if(!this->candy->isConnected()){
        return;
    };

    while(1){
        can_frame frame = this->candy->recieve();
        this->_m.lock();
        this->_frames.push(frame);
        this->_m.unlock();
    };
};

void Pawny::broadcast(){
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
    };
};
