#pragma once

#include "noncopyable.h"
#include <functional>
#include <string>
#include <thread>
#include <memory>
#include <atomic>

namespace dwt {


class Thread : noncopyable {

public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc func, const std::string& name = std::string());

    ~Thread();

    void start();

    void join();

    bool started() const { return started_; }

    pid_t tid() const {return m_tid; }

    const std::string& name() const {return name_; }

    static int numCreated() { return m_numCreated; }

protected:

    void setDefaultName();

    bool started_;
    bool m_joined;

    std::unique_ptr<std::thread> m_thread;

    pid_t m_tid;
    ThreadFunc m_func;

    std::string name_;

    static std::atomic<int> m_numCreated;   // 已创建的线程
};


}