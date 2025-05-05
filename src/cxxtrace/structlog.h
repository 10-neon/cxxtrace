#pragma once
#include <memory>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace neon {

class StructLog {
   public:
    struct CreateOption {
        bool enable_stdout{false};
        std::string file_name{""};
    };
    StructLog(CreateOption const& options);
    ~StructLog() = default;
    static StructLog const& inst() {
        static StructLog inst{CreateOption{true, "cxxtrace.json"}};
        return inst;
    }
    void log(nlohmann::json&& msg) const;

   private:
    std::shared_ptr<spdlog::async_logger> async_logger_;
};
}  // namespace neon
