#include "pawny.h"

#include <linux/can.h>
#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <boost/thread.hpp>

int main(int argc, char** argv){
    Pawny *instance = new Pawny();
    instance->init();

    boost::thread t_canListener(&Pawny::listen, instance);
    boost::thread t_canBroadcaster(&Pawny::broadcast, instance);
    t_canListener.join();
    t_canBroadcaster.join();
}


