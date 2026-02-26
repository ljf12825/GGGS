#pragma once
#include <string>
#include <sys/socket.h>
#include <unistd.h>

class Session {
public:
    int fd{-1};

    explicit Session(int socket_fd) : fd(socket_fd) {}

    virtual ~Session() {
        if (fd >= 0) close(fd);
    }

    bool Send(const std::string& msg) {
        if (fd < 0) return false;

        return send(fd, msg.data(), msg.size(), 0) >= 0;
    }

    virtual void OnReceive(const std::string& msg) {}
};