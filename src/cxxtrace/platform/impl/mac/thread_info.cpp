#include "thread_info.h"

#include <mach/mach.h>
#include <mach/mach_port.h>
#include <mach/thread_act.h>
#include <mach/thread_info.h>
#include <pthread.h>
#include <sys/time.h>

#include <atomic>
#include <cassert>
#include <iostream>

#include "malloc_hook.h"

namespace neon {

class ThreadInfo::Impl {
    Impl() {
        tid_ = next_tid();
        thread_ = pthread_mach_thread_np(pthread_self());
    }

   public:
    ~Impl() {}

    static Impl& current() {
        static thread_local Impl instance;
        return instance;
    }

    std::uint32_t tid() const { return tid_; }
    std::string name() const {
        char name[256]{0};
        pthread_getname_np(pthread_from_mach_thread_np(thread_), name,
                           sizeof(name));
        return name_;
    }
    std::int64_t task_clock_ns() const {
        thread_basic_info_data_t basic_info{};
        assert(MACH_PORT_VALID(thread_));
        mach_msg_type_number_t count = THREAD_BASIC_INFO_COUNT;
        kern_return_t kr =
            thread_info(thread_, THREAD_BASIC_INFO,
                        reinterpret_cast<thread_info_t>(&basic_info), &count);
        assert(kr == KERN_SUCCESS);
        time_value_add(&basic_info.user_time, &basic_info.system_time);
        return basic_info.user_time.seconds * TIME_MICROS_MAX +
               basic_info.user_time.microseconds;
    }
    std::uint64_t allocated_heap_bytes() const { return allocated_bytes_; }
    std::uint64_t deallocated_heap_bytes() const { return deallocated_bytes_; }

    void add_allocate_heap_bytes(std::size_t bytes) {
        allocated_bytes_ += bytes;
    }
    void add_deallocate_heap_bytes(std::size_t bytes) {
        deallocated_bytes_ += bytes;
    }

   private:
    static std::uint32_t next_tid() {
        static std::atomic<std::uint32_t> next_tid_{0};
        next_tid_++;
        return next_tid_;
    }
    std::uint32_t tid_;
    std::string name_;
    thread_port_t thread_;
    std::uint64_t allocated_bytes_{0};
    std::uint64_t deallocated_bytes_{0};
};

class ThreadMemoryStatistics : public MallocListener {
   public:
    ThreadMemoryStatistics() = default;
    static ThreadMemoryStatistics& instance() {
        static ThreadMemoryStatistics instance;
        return instance;
    }

    void alloc(std::size_t bytes) override {
        ThreadInfo::Impl::current().add_allocate_heap_bytes(bytes);
    }
    void dealloc(std::size_t bytes) override {
        ThreadInfo::Impl::current().add_deallocate_heap_bytes(bytes);
    }
};

ThreadInfo::ThreadInfo() : impl_{Impl::current()} {}

ThreadInfo const& ThreadInfo::current() {
    static thread_local ThreadInfo instance;
    return instance;
}
std::uint32_t ThreadInfo::tid() const { return impl_.tid(); }
std::string ThreadInfo::name() const { return impl_.name(); }
std::int64_t ThreadInfo::task_clock_ns() const { return impl_.task_clock_ns(); }
std::uint64_t ThreadInfo::allocated_heap_bytes() const {
    return impl_.allocated_heap_bytes();
}
std::uint64_t ThreadInfo::deallocated_heap_bytes() const {
    return impl_.deallocated_heap_bytes();
}

void ThreadInfo::enable_malloc_statistics() {
    MallocInterposition::setListener(&ThreadMemoryStatistics::instance());
    MallocInterposition::install();
    MallocInterposition::enable();
}
void ThreadInfo::disable_malloc_statistics() {
    MallocInterposition::disable();
    MallocInterposition::setListener(nullptr);
}

}  // namespace neon
