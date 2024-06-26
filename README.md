# dwt_muduo

## 目录树

```
.
|-- CMakeLists.txt
|-- README.md
|-- bin             # test生成可执行文件
|-- build           # 构建目录
|-- lib             # my_muduo生成库
|-- muduo_use       # 使用muduo
|-- src             # 手撕muduo
`-- test            # 测试
```

## 开发顺序

```mermaid
graph TD;

noncopyable --> Logger
Timestamp --> Logger
Logger --> EventLoop

InetAddress --> EventLoop

CurrentThread --> EventLoop

EventLoop --> Poller
EventLoop --> Channel

Poller --> EpollPoller

Thread --> EventLoopThread

EpollPoller --> EventLoopThread
Channel --> EventLoopThread

EventLoopThread --> EventLoopThreadPool

EventLoopThreadPool --> Socket

Socket --> Acceptor
Socket --> Buffer

Buffer --> TcpConnection

Acceptor --> TcpServer
TcpConnection --> TcpServer
```



## 开发中遇到的问题

1. 编译静态库没问题, 编译main_test对Socket测试时报错:
   ![image-20240605180800262](https://github.com/dengwangtao/dwt_muduo/blob/main/README.assets/image-20240605180800262.png?raw=true)
   原因: vscode 的CMake插件没反应过来, 对顶层CMakeLists.txt保存一下, 重新构建项目即可
   ![image-20240605181158978](https://github.com/dengwangtao/dwt_muduo/blob/main/README.assets/image-20240605181158978.png?raw=true)

2. ::accept 调用返回, errno=22

   > 原因: len没有赋值
   > 改正: socklen_t len = static_cast<socklen_t>(sizeof addr);

## 整理

构造TcpServer时 => 构造Acceptor => 创建一个listenFd

TcpServer.start() => Acceptor.listen() => Acceptor.channel.enableReading(); 会将listenFd添加到baseLoop的EpollPoller



构造TcpServer时 => 构造EventLoopThreadPool => 创建一个

TcpServer.start() => EventLoopThreadPool.statr() 创建多个subLoop



baseLoop.loop(); 开启事件循环

baseLoop只监听新连接到达



Acceptor的ReadEventCallback: 为新连接轮询分配一个subLoop

TcpConnection的ReadEventCallback: 处理用户的消息到达回调



