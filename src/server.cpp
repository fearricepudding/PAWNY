#include "server.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <linux/can.h>

Server::Server() {
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    this->setup();
};

Server::~Server() {
    this->shutdown();
};

void sigchld_handler(int s) {
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
};

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    };
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int Server::setup() {
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        std::cout << "[*] getaddrinfo error: " << gai_strerror(rv) << std::endl;
        return 1;
    };

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::cout << "[*] server: socket error" << std::endl;
            continue;
        };

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            std::cout << "[*] setsockopt error" << std::endl;
            exit(1);
        };

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            std::cout << "[*] server: bind error" << std::endl;
            continue;
        };

        break;
    };

    freeaddrinfo(servinfo);

    if (p == NULL) {
        std::cout << "[*] Failed to bind" << std::endl;
        exit(1);
    };

    if (listen(sockfd, BACKLOG) == -1){
        std::cout << "[*] Failed to listen" << std::endl;
        exit(1);
    };

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        std::cout << "[*] sigaction failed" << std::endl;
        exit(1);
    };

    return 0;
};

void Server::waitForConnection() {
    std::cout << "LOOKING" << std::endl;
    while(1){
        std::cout << "WAITING LOOP" << std::endl;
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            std::cout << "[*] Accept error, waiting for new connection" << std::endl;
            continue;
        };

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);

        std::cout << "[*] Connected" << std::endl;
        close(sockfd);
        break;
    };
    std::cout << "RETURNING" << std::endl;
    return;
};

int Server::sendFrame(can_frame packet) {
    if (send(new_fd, &packet, sizeof packet, 0) == -1) {
        std::cout << "Failed to send frame, client disconnect" << std::endl;
        std::cout << errno << std::endl;
        return 1;
    };
    return 0;
};

void Server::shutdown() {
    close(new_fd);
};
