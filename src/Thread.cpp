#include "Thread.h"

#include <semaphore.h>

#include "fmt/format.h"
#include <future>
#include "Logger.h"

#include "CurrentThread.h"

namespace dwt {

std::atomic<int> Thread::m_numCreated{0};

Thread::Thread(ThreadFunc func, const std::string& name)
    : m_started(false),
      m_joined(false),
      m_tid(0),
      m_func(std::move(func)),
      m_name(name) {
  // 构造函数
  setDefaultName();
}

Thread::~Thread() {
  if (m_started && !m_joined) {
    m_thread->detach();
  }
}

void Thread::start() {
  m_started = true;

  std::promise<pid_t> pidPromise;
  auto pidFuture = pidPromise.get_future();

  m_thread = std::make_unique<std::thread>([&]() {

    LOG_INFO("creating thread {}", m_name);
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // 获取系统线程tid
    auto tid = dwt::CurrentThread::tid();
    pidPromise.set_value(tid);
    LOG_INFO("thread created, {} tid={}", m_name, tid);

    m_func();
  });


  m_tid = pidFuture.get();

  // 保证线程获取到线程id后, 该函数才能返回
}

void Thread::join() {
  m_joined = true;

  m_thread->join();
}

void Thread::setDefaultName() {
  int num = ++m_numCreated;
  if (m_name.empty()) {
    m_name = fmt::format("Thread-{}", num);
  }
}

}  // namespace dwt