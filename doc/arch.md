# Architecture

## 设计原则

### 网络层独立

以后可以换

- select
- epoll
- io_uring
- boost.asio

而业务逻辑不改

### Session = Actor 思想

每个客户端

- 有独立状态
- 有消息队列
- 不会直接互相调用

这是现代服务器非常主流的模式

### Dispatcher驱动业务逻辑

统一

```
Message ID
v
Handler Function
v
Business Logic Service
```

## 目录结构

```text
GameServer/

├── Network/
│   ├── SocketServer.h
│   ├── SocketServer.cpp
│   ├── Session.h
│   └── Session.cpp
│
├── Message/
│   ├── Message.h
│   ├── MessageDispatcher.h
│
├── Service/
│   ├── LoginService.h
│   ├── RoomService.h
│
├── Core/
│   ├── GameServer.h
│   ├── GameServer.cpp
│
└── main.cpp
```

## 开发阶段

### 第一阶段

- Epoll Reactor模型
- 二进制协议（自定义Pack/Unpack）
- Connection Pool
- Thread Pool

### 第二阶段

- Actor Message Queue
- Lock-free RingBuffer
- Hot Reload Logic DLL
- Lua Script Service

### 第三阶段

- 分布式GameServer Cluster
- Gateway + Logic Server
- State Synchronization Framework

## v0.1.0

### 架构图

```mermaid
classDiagram
    class TcpServer {
        -int listen_fd
        -unordered_map~int, unique_ptr~Session~~ sessions
        +Init(int port) bool
        +Run()
        -AcceptClient()
        -ReceiveClient(int fd)
    }

    class Session {
        +int fd
        +Send(string msg) bool
        +OnReceive(string msg) virtual
    }

    TcpServer o--> Session : 管理多个客户端连接
```

### 运行流程图

```mermaid
flowchart TD
    A([启动 TcpServer]) --> B[Init 初始化\nsocket / bind / listen]
    B --> C[进入 Run 主循环]

    C --> D[select 监听 socket]
    D --> E{有新连接？}

    E -->|是| F[AcceptClient\n接收客户端]
    F --> G[创建 Session 对象\n存入 sessions 映射表]
    G --> H[打印 Client connected: fd]
    H --> D

    E -->|否| I{客户端有数据？}

    I -->|是| J[ReceiveClient\n接收数据]
    J --> K[recv 读取数据到 buffer]
    K --> L{recv 返回值 ≤ 0？}

    L -->|是| M[从 sessions 中移除客户端\n连接关闭]
    M --> D

    L -->|否| N[创建 string msg = buffer]
    N --> O[调用 sessions fd -\>Send\n'Echo: ' + msg]
    O --> P[向客户端发送回显数据]
    P --> D

    I -->|否| D
```

### 时序图

```mermaid
sequenceDiagram
    participant Client
    participant TcpServer
    participant Session
    
    Client->>TcpServer: connect连接
    TcpServer->>TcpServer: AcceptClient
    TcpServer->>Session: 创建Session对象
    TcpServer-->>Client: 连接建立
    
    Client->>TcpServer: 发送消息 "Hello"
    TcpServer->>TcpServer: ReceiveClient
    TcpServer->>Session: Send("Echo: Hello")
    Session-->>Client: 返回 "Echo: Hello"
    
    Client->>TcpServer: 发送消息 "World"
    TcpServer->>TcpServer: ReceiveClient
    TcpServer->>Session: Send("Echo: World")
    Session-->>Client: 返回 "Echo: World"
    
    Client->>TcpServer: 关闭连接
    TcpServer->>TcpServer: recv返回0
    TcpServer->>TcpServer: sessions.erase(fd)
    TcpServer->>Session: 调用析构函数
    Session->>Session: close(fd)
```

## v0.1.1

### Class Diagram

```mermaid
classDiagram
    class TcpServer {
        -int listen_fd
        -unordered_map~int, unique_ptr~Session~~ sessions
        -MessageDispatcher dispatcher
        +Init(int port) bool
        +Run()
        +RegisterMessage(int msg_id, Handler handler)
        -AcceptClient()
        -ReceiveClient(int fd)
    }

    class Session {
        <<Abstract>>
        #int fd
        #MessageDispatcher* dispatcher
        +Send(string msg) bool
        +OnReceive(string msg) virtual
    }

    class GameSession {
        +OnReceive(string msg) override
    }

    class MessageDispatcher {
        -unordered_map~int, Handler~ handlers
        +Register(int msg_id, Handler handler)
        +Dispatch(int msg_id, Session* session, string payload)
    }

    TcpServer *--> MessageDispatcher : 包含
    TcpServer o--> Session : 管理多个
    Session <|-- GameSession : 继承
    Session --> MessageDispatcher : 使用（回调）
```

### 运行流程图

```mermaid
flowchart TD
    A[启动 TcpServer] --> B[初始化 socket 监听端口]
    B --> C[进入 Run 主循环]
    
    C --> D[使用 select 监听 socket]
    D --> E{监听 socket 可读？}
    E -->|是| F[AcceptClient 接收新连接]
    F --> G[创建 GameSession 并存入 sessions]
    G --> D
    
    E -->|否| H[遍历 sessions 检查数据]
    H --> I{客户端 socket 可读？}
    I -->|是| J[ReceiveClient 接收数据]
    J --> K[调用 Session.OnReceive]
    K --> L[解析消息 ID 和 payload]
    L --> M[MessageDispatcher.Dispatch]
    M --> N[执行注册的回调函数]
    N --> H
    
    I -->|否| H
    H --> D
```

### 时序图

```mermaid
sequenceDiagram
    participant Client
    participant TcpServer
    participant GameSession
    participant MessageDispatcher
    participant Handler

    Client->>TcpServer: 发送消息 "1:hello"
    TcpServer->>GameSession: OnReceive("1:hello")
    GameSession->>GameSession: 解析 msg_id=1, payload="hello"
    GameSession->>MessageDispatcher: Dispatch(1, session, "hello")
    MessageDispatcher->>Handler: 调用 onLogin(session, "hello")
    Handler->>GameSession: Send("Login success")
    GameSession->>Client: 发送响应
```