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
            ("CANFD", "Use can FD")
            ("datarate", boost::program_options::value<int>(), "set the data rate (default disabled)")
            ("port", boost::program_options::value<int>(), "set the listen port (default 8047)")
            ("broadcast", "Broadcast the packets over UDP")
            ("out", boost::program_options::value<std::string>(), "log and store can frames to location");

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
        debug = true;
    };

    Pawny *i_pawny = new Pawny(debug);

    if (vm.count("bitrate")) {
        int bitrate = vm["bitrate"].as<int>(); 
        i_pawny->setBaud(bitrate);
    };

    if (vm.count("broadcast")) {
        int port = 8047;
        if (vm.count("port")) {
            port = vm["port"].as<int>();
        }
        i_pawny->enableBroadcast(port);
    };

    if (vm.count("CANFD")) {
        if (vm.count("datarate")) {
            int datarate = vm["datarate"].as<int>();
            i_pawny->setDataRate(datarate);
        };
        i_pawny->enableFD();
    };

    if (vm.count("out")) {
        std::string storePath = vm["out"].as<std::string>();
        i_pawny->enableLogging(storePath);
    };
    
    i_pawny->init();

    boost::thread_group threads;
    FrameQueue queue;

    boost::thread *t_canListener = new boost::thread(&Pawny::listen, i_pawny, &queue);
    threads.add_thread(t_canListener);

    boost::thread *t_canConsume = new boost::thread(&Pawny::consume, i_pawny, &queue);
    threads.add_thread(t_canConsume);

    threads.join_all();
}


