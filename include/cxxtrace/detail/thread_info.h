#pragma once
#include <cstdint>
#include <string>

namespace cxxtrace {
namespace detail {

class ThreadInfo {
public:
    virtual ~ThreadInfo() = default;
    
    // 新增生命周期追踪接口
    virtual void finalize() noexcept = 0;
    virtual uint64_t start_timestamp() const noexcept = 0;
    virtual uint64_t end_timestamp() const noexcept = 0;
    
    virtual std::string thread_name() const = 0;
    virtual uint64_t thread_id() const = 0;
    virtual uint64_t native_handle() const = 0;
    
    virtual uint64_t cpu_user_time() const = 0;
    virtual uint64_t cpu_system_time() const = 0;
    virtual void update() noexcept = 0;
    // 工厂方法
    static std::unique_ptr<ThreadInfo> current_thread();
};

} // namespace detail
} // namespace cxxtrace