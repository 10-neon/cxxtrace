#include "cxxtrace/location.h"

#include <string.h>

using namespace neon;

const char* SourceLocation::filename() const noexcept {
    const char* end1 = ::strrchr(filepath_, '/');
    const char* end2 = ::strrchr(filepath_, '\\');
    if (!end1 && !end2) {
        return filepath_;
    } else {
        return (end1 > end2) ? end1 + 1 : end2 + 1;
    }
}
std::string SourceLocation::to_string() const {
    char buf[256];
    snprintf(buf, sizeof(buf), "%s:%d", filename(), line());
    return buf;
}
