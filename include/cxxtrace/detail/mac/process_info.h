#pragma once
#include "../process_info.h"

namespace cxxtrace {
namespace detail {

class MacProcessInfo final : public ProcessInfo {
public:
    MacProcessInfo();
    
    std::string process_name() const override;
    int64_t process_id() const override;
    int64_t physical_memory() const override;
    int64_t resident_size() const override;
    int64_t virtual_size() const override;
    int64_t cpu_user_time() const override;
    int64_t cpu_system_time() const override;
    int64_t thread_count() const override;
    int64_t start_timestamp() const override;

private:
    int64_t pid_{0};
    std::string name_;
};

} // namespace detail
} // namespace cxxtrace