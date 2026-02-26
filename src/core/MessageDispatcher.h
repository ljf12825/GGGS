#pragma once
#include "Session.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <utility>

class Session;

class MessageDispatcher {
public:
    using Handler = std::function<void(Session*, const std::string&)>;

private:
    std::unordered_map<int, Handler> handlers;

public:
    void Register(int msg_id, Handler handler) {
        handlers[msg_id] = std::move(handler);
    }

    void Dispatch(int msg_id, Session* session, const std::string& payload) {
        auto it = handlers.find(msg_id);
        if (it != handlers.end()) it->second(session, payload);
    }
};