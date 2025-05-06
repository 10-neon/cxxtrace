#pragma once
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

struct TraceContext {
    Location loc;
    Tag tag;
    static void before(TraceContext const& ctx) {
        TraceSectionBegin(ctx.tag, ctx.loc);
    }
    static void after(TraceContext const& ctx) {
        TraceSectionEnd(ctx.tag, ctx.loc);
    }
};
template <typename Pointer>
using TracePtr = WrapPtr<Pointer, decltype(TraceContext::before)*,
                         decltype(TraceContext::after)*, TraceContext>;

template <typename Pointer>
TracePtr<Pointer> traceWrap(Tag tag, SourceLocation loc,
                            Pointer pointer = nullptr) {
    return wrap(std::move(pointer), &TraceContext::before, &TraceContext::after,
                TraceContext{.loc = loc, .tag = tag});
}

}  // namespace neon

#define TRACE_SCOPE(tag)                       \
    ::neon::TraceScope tag##_trace_scope(#tag, \
                                         ::neon::SourceLocation::current());
