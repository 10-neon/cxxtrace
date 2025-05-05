#include "thread_info.h"

#include <pthread.h>

#include <atomic>
#include <iostream>

#include "malloc_hook.h"
#include "perf_event.h"

namespace neon {

class ThreadInfo::Impl {
    Impl() {
        tid_ = next_tid();
        thread_ = pthread_self();
        event_ = PerfEvent::create(PerfEvent::TypeID::SOFTWARE,
                                   PerfEvent::Config::SW_TASK_CLOCK,
                                   PerfEvent::Domain::ALL);
        if (event_) {
            event_->enable();
        }
    }

   public:
    ~Impl() {
        if (event_) {
            event_->disable();
        }
    }

    static Impl& current() {
        static thread_local Impl instance;
        return instance;
    }

    std::uint32_t tid() const { return tid_; }
    std::string name() const {
        char name[256]{0};
        pthread_getname_np(thread_, name, sizeof(name));
        return name_;
    }
    std::int64_t task_clock_ns() const {
        PerfEvent::Count count;
        if (event_ && event_->now(count)) {
            return count.value;
        }
        return 0;
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
        static std::atomic<std::uint32_t> next_tid_{1};
        return next_tid_.fetch_add(1, std::memory_order::memory_order_relaxed);
    }
    std::unique_ptr<PerfEvent> event_;
    std::uint32_t tid_;
    std::string name_;
    pthread_t thread_;
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
    if (!MallocInterposition::install()) {
        std::cerr << "enable malloc statistics fail" << std::endl;
    };
}
void ThreadInfo::disable_malloc_statistics() {
    MallocInterposition::setListener(nullptr);
}

}  // namespace neon
