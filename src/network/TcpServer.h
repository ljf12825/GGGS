#include <unordered_map>
#include <memory>
#include <utility>
#include "../core/Session.h"
#include "../core/MessageDispatcher.h"

class TcpServer {
private:
    int listen_fd{-1};

    std::unordered_map<int, std::unique_ptr<Session>> sessions;

    MessageDispatcher dispatcher;

    void AcceptClient();
    void ReceiveClient(int fd);

public:
    virtual ~TcpServer();

    bool Init(int port);

    void Run();

    template<typename Handler>
    void RegisterMessage(int msg_id, Handler&& handler) {
        dispatcher.Register(msg_id, std::forward<Handler>(handler));
    }
};