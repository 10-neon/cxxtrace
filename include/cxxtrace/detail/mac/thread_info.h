#pragma once
#include "../../detail/thread_info.h"
#include <mach/mach.h>

namespace cxxtrace {
namespace detail {

class MacThreadInfo : public ThreadInfo {
public:
    explicit MacThreadInfo(thread_port_t thread);
    void update() noexcept override; // 新增方法
    
    std::string thread_name() const override;
    uint64_t thread_id() const override;
    uint64_t native_handle() const override;
    uint64_t cpu_user_time() const override;
    uint64_t cpu_system_time() const override;
    
    // 实现基类接口
    void finalize() noexcept override;
    uint64_t start_timestamp() const noexcept override;
    uint64_t end_timestamp() const noexcept override;

private:
    thread_port_t thread_;
    std::uint64_t thread_id_;
    mutable std::string name_cache_;
    mutable uint64_t last_update_; // 最后更新时间戳（毫秒）
    bool valid_;                   // 线程是否有效
    thread_basic_info_data_t basic_info_{};
    uint64_t start_time_{0};
    uint64_t end_time_{0};
};

} // namespace detail
} // namespace cxxtrace