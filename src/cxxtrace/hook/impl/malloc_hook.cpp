#include "malloc_hook.h"

namespace neon {

std::atomic<MallocListener *> MallocInterposition::s_listener{nullptr};
std::atomic<bool> MallocInterposition::s_enable{false};
std::once_flag MallocInterposition::s_install_once;

}  // namespace neon
