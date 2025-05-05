#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace neon {

class ThreadInfo {
   public:
    class Impl;
    static ThreadInfo const& current();
    std::uint32_t tid() const;
    std::string name() const;
    std::int64_t task_clock_ns() const;
    std::uint64_t allocated_heap_bytes() const;
    std::uint64_t deallocated_heap_bytes() const;
    static void enable_malloc_statistics();
    static void disable_malloc_statistics();

    ~ThreadInfo() = default;

   protected:
    ThreadInfo();
    ThreadInfo(ThreadInfo&& other) = delete;
    ThreadInfo& operator=(ThreadInfo&& other) = delete;
    ThreadInfo(ThreadInfo const& other) = delete;
    ThreadInfo& operator=(ThreadInfo const& other) = delete;

   private:
    Impl& impl_;
};

}  // namespace neon
