#include <pthread.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/resource.h>
// #include "cxxtrace/platform/platform.h"
#include <string>

#include "platform.h"

namespace neon {
namespace platform {

std::string name() { return "Linux"; }
}  // namespace platform
}  // namespace neon
