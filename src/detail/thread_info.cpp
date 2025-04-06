#include "cxxtrace/detail/thread_info.h"
#ifdef __APPLE__
#include "cxxtrace/detail/mac/thread_info.h"
#include <pthread.h>
#endif

namespace cxxtrace {
namespace detail {

std::unique_ptr<ThreadInfo> ThreadInfo::current_thread() {
#ifdef __APPLE__
    auto thread = pthread_mach_thread_np(pthread_self());
    return std::make_unique<MacThreadInfo>(thread);
#else
    return nullptr; // 其他平台暂不实现
#endif
}

} // namespace detail
} // namespace cxxtrace