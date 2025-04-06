#include "cxxtrace/detail/mac/thread_info.h"
#include <pthread.h>
#include <mach/thread_act.h>
#include <mach/thread_info.h>
#include <mach/mach_port.h>
#include <sys/time.h>

namespace cxxtrace {
namespace detail {

// 修改构造函数
MacThreadInfo::MacThreadInfo(thread_port_t thread)
    : thread_(thread)
    , last_update_{0}
    , valid_{true} 
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    start_time_ = tv.tv_sec * 1000ULL + tv.tv_usec / 1000; // 毫秒级时间戳
    thread_id_ = static_cast<uint64_t>(pthread_mach_thread_np(
        pthread_from_mach_thread_np(thread_)));
}

// 新增update方法
void MacThreadInfo::update() noexcept {
    if (!valid_) return;
    
    mach_msg_type_number_t count = THREAD_BASIC_INFO_COUNT;
    kern_return_t kr = thread_info(thread_, THREAD_BASIC_INFO,
                                  reinterpret_cast<thread_info_t>(&basic_info_), &count);
    
    // 检查线程是否存活
    if (kr != KERN_SUCCESS || !MACH_PORT_VALID(thread_)) {
        valid_ = false;
        thread_ = THREAD_NULL;
        return;
    }
    
    // 更新线程名缓存（带TTL）
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    auto now = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    if (now - last_update_ > 1000) { // TTL设为1秒
        name_cache_.clear();
        last_update_ = now;
    }
}


std::string MacThreadInfo::thread_name() const {
    if (name_cache_.empty()) {
        char name[256]{0};
        if (valid_) {
            pthread_getname_np(pthread_from_mach_thread_np(thread_), name, sizeof(name));
        }
        name_cache_ = name;
    }
    return name_cache_;
}

// 其他方法增加有效性检查
uint64_t MacThreadInfo::thread_id() const {
    return thread_id_;
}

uint64_t MacThreadInfo::cpu_user_time() const {
    return basic_info_.user_time.microseconds;
}

uint64_t MacThreadInfo::native_handle() const {
    return static_cast<uint64_t>(thread_);
}

uint64_t MacThreadInfo::cpu_system_time() const {
    return basic_info_.system_time.microseconds;
}

// 新增finalize方法
void MacThreadInfo::finalize() noexcept {
    this->update();
    char name[256]{0};
    pthread_getname_np(pthread_from_mach_thread_np(thread_), name, sizeof(name));
    name_cache_ = name;
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    end_time_ = tv.tv_sec * 1000ULL + tv.tv_usec / 1000;
}

uint64_t MacThreadInfo::start_timestamp() const noexcept {
    return start_time_;
}

// 新增结束时间获取方法（根据需求）
uint64_t MacThreadInfo::end_timestamp() const noexcept {
    return end_time_;
}

} // namespace detail
} // namespace cxxtrace