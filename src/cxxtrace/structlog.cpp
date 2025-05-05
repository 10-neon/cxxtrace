#include "structlog.h"

#include <cstdio>

namespace neon {

StructLog::StructLog(CreateOption const& options) {
    std::vector<spdlog::sink_ptr> sinks;
    if (options.enable_stdout) {
        sinks.emplace_back(
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }
    if (!options.file_name.empty()) {
        spdlog::file_event_handlers handlers;
        ;
        handlers.after_open = [](spdlog::filename_t filename,
                                 std::FILE* fstream) { fputs("[\n", fstream); };
        handlers.before_close = [](spdlog::filename_t filename,
                                   std::FILE* fstream) {
            fputs("{}]", fstream);
        };
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            options.file_name, true, handlers);
        sinks.emplace_back(file_sink);
        file_sink->set_pattern("    %v,");
    }
    if (sinks.empty()) {
        return;
    }
    spdlog::init_thread_pool(8192, 1);
    async_logger_ = std::make_shared<spdlog::async_logger>(
        "cxxtrace", sinks.begin(), sinks.end(), spdlog::thread_pool(),
        spdlog::async_overflow_policy::block);
}

void StructLog::log(nlohmann::json&& msg) const {
    if (async_logger_) {
        async_logger_->info(msg.dump());
    }
}
}  // namespace neon
