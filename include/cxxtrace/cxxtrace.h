#pragma once
#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

#include "location.h"
#include "wrap.hpp"

namespace neon {

using Tag = const char*;

void TraceEnable();
void TraceDisable();
void TraceSectionBegin(Tag tag, const Location& loc);
void TraceSectionEnd(Tag tag, const Location& loc);

class TraceScope {
   public:
    TraceScope(Tag tag, const Location& loc) : tag_{tag}, loc_{loc} {
        TraceSectionBegin(tag_, loc_);
    }
    ~TraceScope() { TraceSectionEnd(tag_, loc_); }

   private:
    const Tag tag_;
    Location loc_;
};

struct TraceInfo {
    Location loc;
    Tag tag;
};
template <typename Pointer>
using TracePtr = ParametricWrapPtr<Pointer, void (*)(TraceInfo const&),
                                   void (*)(TraceInfo const&), TraceInfo>;

template <typename Pointer>
TracePtr<Pointer> traceWrap(Tag tag, SourceLocation loc,
                            Pointer pointer = nullptr) {
    return TracePtr<Pointer>{
        std::move(pointer),
        [](TraceInfo const& trace_info) {
            TraceSectionBegin(trace_info.tag, trace_info.loc);
        },
        [](TraceInfo const& trace_info) {
            TraceSectionEnd(trace_info.tag, trace_info.loc);
        },
        TraceInfo{.loc = loc, .tag = tag}};
}

}  // namespace neon

#define TRACE_SCOPE(tag)                       \
    ::neon::TraceScope tag##_trace_scope(#tag, \
                                         ::neon::SourceLocation::current());
