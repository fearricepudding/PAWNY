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
            ("debug", "start in debug mode");

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

    bool debug = false;

    if (vm.count("debug")) {
        std::cout << "Starting in debug mode" << std::endl;
        debug = true;
    };

    Pawny *instance = new Pawny(debug);
    instance->init();

    boost::thread t_canListener(&Pawny::listen, instance);
    boost::thread t_canBroadcaster(&Pawny::broadcast, instance);
    t_canListener.join();
    t_canBroadcaster.join();
}


