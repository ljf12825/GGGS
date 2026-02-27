#pragma once
#include "MessageDispatcher.h"
#include <string>
#include <sys/socket.h>
#include <unistd.h>

class Session {
protected:
    int fd{-1};
    MessageDispatcher* dispatcher{nullptr};

public:
    Session(int socket_fd, MessageDispatcher* disp = nullptr)
        : fd(socket_fd), dispatcher(disp) {}
    
    virtual ~Session() {
        if (fd >= 0) close(fd);
    }

    bool Send(const std::string& msg) {
        if (fd < 0) return false;
        return send(fd, msg.data(), msg.size(), 0) >= 0;
    }

    virtual void OnReceive(const std::string& msg) {
        size_t pos = msg.find(':');
        if (pos != std::string::npos && dispatcher) {
            int msg_id = std::stoi(msg.substr(0, pos));
            std::string payload = msg.substr(pos + 1);
            dispatcher->Dispatch(msg_id, this, payload);
        }
    }
};