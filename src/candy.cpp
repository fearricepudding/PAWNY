#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <string>
#include <iostream>
#include <sstream>

#include "candy.h"
#include "Logger.h"

using namespace fear;


Candy::Candy(bool debug, int bitrate, Logger *logger) {
    this->logger = logger;
    if (debug) {
        this->logger->add("[DEBUG] Starting candy in DEBUG mode");
    } else {
        this->logger->add("[*] Starting candy");
    };
    this->_debug = debug;
    this->error = "";
    this->_connected = false;
    this->_bitrate = bitrate;
    this->_fd = false;
}

Candy::Candy(bool debug, int bitrate, int datarate, Logger* logger) : Candy(debug, bitrate, logger) {
    this->_fd = true;
    this->_datarate = datarate;
}

Candy::~Candy() {
    this->closeCanLink();
};

void Candy::setup() {
    this->logger->add("[*] Starting can link");
    if (this->_debug) {
        this->logger->add("[DEBUG] skipping setup");
        this->_connected = true;
        return;
    };
    int status = this->setupCanLink();
    if(status != 0){
        this->logger->add(this->error);
    };
};

void Candy::shutdown() {
    this->logger->add("[*] Closing can link");
    if (this->_debug) {
        return;
    };
    this->closeCanLink();
};

std::string Candy::getError() {
    return this->error;
};

int Candy::setupCanLink() {
    struct sockaddr_can addr;
    struct ifreq ifr;
    int ret;

    /**
     * TODO: 
     *          - check different bitrate speeds for connection
     *          - connect with speed
     *          - gateway?
     *          - send recieve
     *          - find modules
     *          - store data
     */

    std::stringstream setupCommand;
    this->logger->add("[*] Setting up can0 link with bitrate "+ std::to_string(this->_bitrate));
    setupCommand << "sudo ip link set can0 type can bitrate " << std::to_string(this->_bitrate);
    if (this->_fd) {
        setupCommand << " dbitrate " << this->_datarate << " fd on";
    };
    this->logger->add("[DEBUG] " + setupCommand.str());
    system(setupCommand.str().c_str());
    system("sudo ifconfig can0 up");

    this->s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (this->s < 0) {
        this->error = "Socket failure";
        return 1;
    };

    strcpy(ifr.ifr_name, "can0");
    ret = ioctl(this->s, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        this->error = "Device failure";
        return 1;
    };

    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(this->s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        this->error = "Bind failure";
        return 1;
    };

    this->_connected = true;
    return 0;
}

void Candy::closeCanLink() {
    close(this->s);
    system("sudo ifconfig can0 down");
};

bool Candy::isConnected(){
    return this->_connected;
};

canfd_frame Candy::recieve() {
    struct canfd_frame frame;
    memset(&frame, 0, sizeof(frame));

    if (this->_debug) {
        // Send empty frame
        // TODO: Build random frame
        frame.can_id = 0x500;
        //frame.can_dlc = 8;
        frame.data[0] = 122;
        frame.data[1] = 150;
        frame.data[2] = -27;
        frame.data[3] = -22;
        frame.data[4] = 160;
        frame.data[5] = 160;
        frame.data[6] = -60;
        frame.data[7] = 90;
        return frame;
    };

    //4.Define receive rules
    //struct can_filter rfilter[1];
    //rfilter[0].can_id = 0x123;
    //rfilter[0].can_mask = CAN_SFF_MASK;
    //setsockopt(this->s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    int nbytes;
    while(1) {
        nbytes = read(this->s, &frame, sizeof(frame));
        if(nbytes > 0) {
            int i = 0;
            return frame;
        };
    };
};

int Candy::send() {
    int nbytes;
    struct canfd_frame frame;
    memset(&frame, 0, sizeof(struct canfd_frame));

    //4.Disable filtering rules, do not receive packets, only send
    setsockopt(this->s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    //5.Set send data
    frame.can_id = 0x123;
    frame.data[0] = 1;
    frame.data[1] = 2;
    frame.data[2] = 3;
    frame.data[3] = 4;
    frame.data[4] = 5;
    frame.data[5] = 6;
    frame.data[6] = 7;
    frame.data[7] = 8;

    //6.Send message
    nbytes = write(this->s, &frame, sizeof(frame));
    if(nbytes != sizeof(frame)) {
        printf("Send Error frame[0]!\r\n");
        system("sudo ifconfig can0 down");
    }

    return 0;
}
