#include "cxxtrace/detail/process_info.h"
#ifdef __APPLE__
#include "cxxtrace/detail/mac/process_info.h"
#endif

namespace cxxtrace {
namespace detail {

std::unique_ptr<ProcessInfo> ProcessInfo::create() {
#ifdef __APPLE__
    return std::make_unique<MacProcessInfo>();
#else
    return nullptr; // 其他平台暂不实现
#endif
}

} // namespace detail
} // namespace cxxtrace