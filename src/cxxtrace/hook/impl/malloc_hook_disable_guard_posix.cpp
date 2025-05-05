#include <mutex>

#include "malloc_hook_disable_guard.h"

namespace neon {

static pthread_key_t s_key;
static std::once_flag s_install_once;

MallocHookDisableGuard::MallocHookDisableGuard() {
    origin_ = pthread_getspecific(s_key);
    pthread_setspecific(s_key, this);
}
MallocHookDisableGuard::~MallocHookDisableGuard() {
    pthread_setspecific(s_key, origin_);
}

bool MallocHookDisableGuard::isDisable() {
    return pthread_getspecific(s_key) ? true : false;
}

void MallocHookDisableGuard::install() {
    std::call_once(s_install_once,
                   []() { pthread_key_create(&s_key, nullptr); });
}

}  // namespace neon
