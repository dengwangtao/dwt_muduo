#include "CurrentThread.h"



namespace dwt{

namespace CurrentThread {

thread_local int t_cacheTid = 0;


// 通过系统调用获取当前线程tid值
void cachedTid() {
        // 平台特定的实现
#ifdef __linux__
        // Linux：使用 syscall 获取系统线程ID
        t_cacheTid = static_cast<pid_t>(::syscall(SYS_gettid));
#elif defined(_WIN32)
        // Windows：使用 GetCurrentThreadId 获取线程ID
        t_cacheTid = static_cast<pid_t>(::GetCurrentThreadId());
#elif defined(__APPLE__) || defined(__MACH__)
        // macOS：使用 pthread_self 获取线程ID
        t_cacheTid = static_cast<pid_t>(::pthread_self());
#else
        // 其他平台，默认返回 0
        t_cacheTid = 0;
#endif
}


} // end CurrentThread dwt


} // end namespace dwt