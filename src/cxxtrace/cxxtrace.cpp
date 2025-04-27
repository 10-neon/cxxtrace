#include "cxxtrace/cxxtrace.h"

#include <string>
#include <tl/expected.hpp>

#include "platform.h"

namespace neon {

std::string hello_world() {
    return std::string("Hello World!") + platform::name();
}

}  // namespace neon
