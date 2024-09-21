#include "pawny.h"

#include <linux/can.h>
#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <boost/thread.hpp>

#include <boost/program_options.hpp>

int main(int argc, char** argv){
    boost::program_options::variables_map vm;
    boost::program_options::options_description desc("Allowed options");

    try{
        desc.add_options()
            ("help", "produce help message")
            ("debug", "start in debug mode")
            ("bitrate", boost::program_options::value<int>(), "set the CAN bitrate (default 10400)")
            ("port", boost::program_options::value<int>(), "set the listen port (default 8047)");


        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);

    } catch(std::exception &e) {
        std::cerr << "error: " << e.what() << "\n" << "For help use ./PAWNY --help \n";
        return 1;
    };
    
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    };

    int bitrate = 10400;
    if (vm.count("bitrate")) {
        bitrate = vm["bitrate"].as<int>(); 
    };

    int port = 8047;
    if (vm.count("port")) {
        port = vm["port"].as<int>();
    }

    bool debug = false;
    if (vm.count("debug")) {
        debug = true;
    };

    Pawny *instance = new Pawny(debug, bitrate, port);
    instance->init();

    FrameQueue queue;

    boost::thread t_canListener(&Pawny::listen, instance, &queue);
    boost::thread t_canBroadcaster(&Pawny::broadcast, instance, &queue, port);
    t_canListener.join();
    t_canBroadcaster.join();
}


