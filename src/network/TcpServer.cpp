#include "TcpServer.h"

#include <algorithm>
#include <asm-generic/socket.h>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <cstring>
#include <unistd.h>
#include <vector>

TcpServer::~TcpServer() {
    if (listen_fd >= 0) close(listen_fd);
}

bool TcpServer::Init(int port)
{
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) return false;

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) return false;

    if (listen(listen_fd, 64) < 0) return false;

    return true;
}

void TcpServer::AcceptClient() {
    sockaddr_in client{};
    socklen_t len = sizeof(client);

    int fd = accept(listen_fd, (sockaddr*)&client, &len);
        
    if (fd < 0) return;
        
    sessions[fd] = std::make_unique<Session>(fd, &dispatcher);
        
    std::cout << "Client connected: " << fd << "\n";
}

void TcpServer::ReceiveClient(int fd) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int ret = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (ret <= 0)
    {
        sessions.erase(fd);
        return;
    }

    std::string msg(buffer, ret);

    if (sessions[fd]) {
        sessions[fd]->OnReceive(msg);
    }
}

void TcpServer::Run() {
    while (true) {
        fd_set read_set;
        FD_ZERO(&read_set);

        FD_SET(listen_fd, &read_set);

        int max_fd = listen_fd;

        for (auto& kv : sessions) {
            FD_SET(kv.first, &read_set);
            max_fd = std::max(max_fd, kv.first);
        }

        select(max_fd + 1, &read_set, nullptr, nullptr, nullptr);

        if (FD_ISSET(listen_fd, &read_set)) AcceptClient();

        std::vector<int> to_recv;

        for (auto& kv : sessions) {
            if (FD_ISSET(kv.first, &read_set)) to_recv.push_back(kv.first);
        }

        for (auto fd : to_recv) ReceiveClient(fd);
    }
}