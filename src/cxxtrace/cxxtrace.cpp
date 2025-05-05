#include "cxxtrace/cxxtrace.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <tl/expected.hpp>

#include "nlohmann/json.hpp"
#include "structlog.h"
#include "thread_info.h"

namespace neon {

struct TraceEvent {
    enum class Type {
        kScopeBegin,
        kScopeEnd,
    };
    TraceEvent() noexcept = default;
    TraceEvent(TraceEvent&& other) noexcept = default;
    TraceEvent(TraceEvent const& other) noexcept = default;
    TraceEvent& operator=(TraceEvent&& other) noexcept = default;
    TraceEvent& operator=(TraceEvent const& other) noexcept = default;

    TraceEvent(Type type, Tag tag, const Location& loc)
        : type{type}, tag{tag}, loc{loc} {}
    Type type{Type::kScopeBegin};
    Tag tag{nullptr};
    Location loc{};
    std::uint32_t tid{0};
    std::int64_t task_clock_ns{0};
    std::int64_t allocated_heap_bytes{0};
    std::int64_t deallocated_heap_bytes{0};
    std::int64_t ts_ns{0};
};

static std::int64_t now_timestamp_ns() {
    auto now = std::chrono::steady_clock::now();
    auto nanos = std::chrono::time_point_cast<std::chrono::nanoseconds>(now)
                     .time_since_epoch();
    return static_cast<std::int64_t>(nanos.count());
}

static std::atomic<bool> g_trace_enabled_{false};
void TraceEnable() {
    g_trace_enabled_ = true;
    ThreadInfo::enable_malloc_statistics();
}

void TraceDisable() {
    ThreadInfo::disable_malloc_statistics();
    g_trace_enabled_ = false;
}

static nlohmann::json to_json(TraceEvent const& event) {
    nlohmann::json json;
    json["event"] = (event.type == TraceEvent::Type::kScopeBegin) ? "B" : "E";
    json["tag"] = event.tag ? event.tag : "";
    json["file"] = event.loc.filename();
    json["line"] = event.loc.line();
    json["tid"] = event.tid;
    json["task_clock"] = event.task_clock_ns;
    json["alloc"] = event.allocated_heap_bytes;
    json["dealloc"] = event.deallocated_heap_bytes;
    json["ts"] = event.ts_ns;
    return json;
}

void TraceSectionBegin(Tag tag, const Location& loc) {
    if (!g_trace_enabled_) {
        return;
    }
    TraceEvent event{TraceEvent::Type::kScopeBegin, tag, loc};
    event.tid = ThreadInfo::current().tid();
    event.task_clock_ns = ThreadInfo::current().task_clock_ns();
    event.allocated_heap_bytes = ThreadInfo::current().allocated_heap_bytes();
    event.deallocated_heap_bytes =
        ThreadInfo::current().deallocated_heap_bytes();
    event.ts_ns = now_timestamp_ns();
    StructLog::inst().log(to_json(event));
}

void TraceSectionEnd(Tag tag, const Location& loc) {
    if (!g_trace_enabled_) {
        return;
    }

    TraceEvent event{TraceEvent::Type::kScopeEnd, tag, loc};
    event.tid = ThreadInfo::current().tid();
    event.task_clock_ns = ThreadInfo::current().task_clock_ns();
    event.allocated_heap_bytes = ThreadInfo::current().allocated_heap_bytes();
    event.deallocated_heap_bytes =
        ThreadInfo::current().deallocated_heap_bytes();
    event.ts_ns = now_timestamp_ns();
    StructLog::inst().log(to_json(event));
}

}  // namespace neon
