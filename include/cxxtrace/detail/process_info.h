#pragma once
#include <cstdint>
#include <string>

namespace cxxtrace {
namespace detail {

class ProcessInfo {
public:
    virtual ~ProcessInfo() = default;
    
    // 基础信息
    virtual std::string process_name() const = 0;
    virtual int64_t process_id() const = 0;
    
    // 内存信息（单位：bytes）
    virtual int64_t physical_memory() const = 0;
    virtual int64_t resident_size() const = 0;
    virtual int64_t virtual_size() const = 0;

    // 工厂方法
    static std::unique_ptr<ProcessInfo> create();
    
    // CPU 时间（微秒）
    virtual int64_t cpu_user_time() const = 0;
    virtual int64_t cpu_system_time() const = 0;
    virtual int64_t thread_count() const = 0;
    virtual int64_t start_timestamp() const = 0; // 毫秒时间戳
};

} // namespace detail
} // namespace cxxtrace